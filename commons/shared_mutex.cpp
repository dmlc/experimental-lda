#include "shared_mutex.h"

template <class Clock, class Duration>
bool shared_mutex::try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time)
{
	std::unique_lock<mutex_t> lk(mut_);
	if (state_ & write_entered_)
	{
		while (true)
		{
			std::cv_status status = gate1_.wait_until(lk, abs_time);
			if ((state_ & write_entered_) == 0)
				break;
			if (status == std::cv_status::timeout)
				return false;
		}
	}
	state_ |= write_entered_;
	if (state_ & n_readers_)
	{
		while (true)
		{
			std::cv_status status = gate2_.wait_until(lk, abs_time);
			if ((state_ & n_readers_) == 0)
				break;
			if (status == std::cv_status::timeout)
			{
				state_ &= ~write_entered_;
				return false;
			}
		}
	}
	return true;
}

template <class Clock, class Duration>
bool shared_mutex::try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& abs_time)
{
	std::unique_lock<mutex_t> lk(mut_);
	if ((state_ & write_entered_) || (state_ & n_readers_) == n_readers_)
	{
		while (true)
		{
			std::cv_status status = gate1_.wait_until(lk, abs_time);
			if ((state_ & write_entered_) == 0 &&
				(state_ & n_readers_) < n_readers_)
				break;
			if (status == std::cv_status::timeout)
				return false;
		}
	}
	count_t num_readers = (state_ & n_readers_) + 1;
	state_ &= ~n_readers_;
	state_ |= num_readers;
	return true;
}

// shared_mutex

shared_mutex::shared_mutex()
	: state_(0)
{
}

shared_mutex::~shared_mutex()
{
	std::lock_guard<mutex_t> _(mut_);
}

// Exclusive ownership

void
shared_mutex::lock()
{
	std::unique_lock<mutex_t> lk(mut_);
	while (state_ & write_entered_)
		gate1_.wait(lk);
	state_ |= write_entered_;
	while (state_ & n_readers_)
		gate2_.wait(lk);
}

bool
shared_mutex::try_lock()
{
	std::unique_lock<mutex_t> lk(mut_);
	if (state_ == 0)
	{
		state_ = write_entered_;
		return true;
	}
	return false;
}

void
shared_mutex::unlock()
{
	std::lock_guard<mutex_t> _(mut_);
	state_ = 0;
	gate1_.notify_all();
}

// Shared ownership

void
shared_mutex::lock_shared()
{
	std::unique_lock<mutex_t> lk(mut_);
	while ((state_ & write_entered_) || (state_ & n_readers_) == n_readers_)
		gate1_.wait(lk);
	count_t num_readers = (state_ & n_readers_) + 1;
	state_ &= ~n_readers_;
	state_ |= num_readers;
}

bool
shared_mutex::try_lock_shared()
{
	std::unique_lock<mutex_t> lk(mut_);
	count_t num_readers = state_ & n_readers_;
	if (!(state_ & write_entered_) && num_readers != n_readers_)
	{
		++num_readers;
		state_ &= ~n_readers_;
		state_ |= num_readers;
		return true;
	}
	return false;
}

void
shared_mutex::unlock_shared()
{
	std::lock_guard<mutex_t> _(mut_);
	count_t num_readers = (state_ & n_readers_) - 1;
	state_ &= ~n_readers_;
	state_ |= num_readers;
	if (state_ & write_entered_)
	{
		if (num_readers == 0)
			gate2_.notify_one();
	}
	else
	{
		if (num_readers == n_readers_ - 1)
			gate1_.notify_one();
	}
}