/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/log/common.hpp>
#include <boost/log/sinks/syslog_backend.hpp>

#include <memory>
#include <type_traits>

template <class T> class Key {
public:
	static_assert(!std::is_reference<T>::value, "");
	Key() = default;
	Key(Key const &) = delete;
	Key(Key &&) = delete;
	Key &operator=(Key const &) = delete;
	Key &operator=(Key &&) = delete;
};

class Context {
private:

	class Anything {
	public:
		virtual ~Anything() = default;
		virtual void *get_value() = 0;
	};

	template <class T> class Something : public Anything {

		static_assert(std::is_same<typename std::decay<T>::type, T>::value, "");

	public:
		Something(T &&value) : _value(std::move(value)) {}
		void *get_value() override { return &_value; }
	private:
		T _value;
	};

	struct Data {
		std::shared_ptr<const Data> _parent;
		const void *_key;
		std::unique_ptr<Anything> _value;
	};

	std::shared_ptr<const Data> _data;

	Context(std::shared_ptr<const Data> data) : _data(std::move(data))
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	}

public:

	static Context get_empty();
	static const Context &get_current();
	static Context swap_current(Context other);
	static boost::log::sources::severity_logger<int> _logger;

	Context() = default;
	Context(Context const &) = delete;
	Context(Context &&) = default;
	Context &operator=(Context const &) = delete;
	Context &operator=(Context &&) = default;

	template <class T> const T *get_value(const Key<T> &key) const
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		for (auto *it = this->_data.get(); it != nullptr; it = it->_parent.get())
		{
			if (it->_key == &key)
			{
				return static_cast<const T *>(it->_value->get_value());
			}
		}
		return nullptr;
	}

	template <class T> const T &get_existing(const Key<T> &key) const
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		auto value = get_value(key);
		assert(value && "key does not exist");
		return *value;
	}

	template <class T> Context derive(const Key<T> &key, typename std::decay<T>::type value) const &
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		return Context(std::make_shared<Data>(Data{_data, &key, std::make_unique<Something<typename std::decay<T>::type>>(std::move(value))}));
	}

	template <class T> Context derive(const Key<T> &key, typename std::decay<T>::type value) &&
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		return Context(std::make_shared<Data>(Data{std::move(_data), &key, std::make_unique<Something<typename std::decay<T>::type>>(std::move(value))}));
	}

	template <class T> Context derive(T &&value) const &
	{
		static Key<typename std::decay<T>::type> _private_key;
		return derive(_private_key, std::forward<T>(value));
	}

	template <class T> Context derive(T &&value) &&
	{
		static Key<typename std::decay<T>::type> _private_key;
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		return std::move(this)->derive(_private_key, std::forward<T>(value));
	}

	Context clone() const
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		return Context(_data);
	}

};

class Scoped_context {
public:
	Scoped_context(Context context) : _previous(Context::swap_current(std::move(context)))
	{
		BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	}

	~Scoped_context()
	{
		BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
		Context::swap_current(std::move(_previous));
	}
	Scoped_context(Scoped_context const &) = delete;
	Scoped_context(Scoped_context &&) = delete;
	Scoped_context &operator=(Scoped_context const &) = delete;
	Scoped_context &operator=(Scoped_context &&) = delete;

private:
	Context _previous;
};

class Scoped_context_with_value {
public:
	template <typename T> Scoped_context_with_value(const Key<T> &key, typename std::decay<T>::type value)
		: _previous(Context::get_current().derive(key, std::move(value)))
	{
		BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	}

	// Anonymous values can be used for the destructor side-effect.
	template <typename T> Scoped_context_with_value(T &&value)
		: _previous(Context::get_current().derive(std::forward<T>(value)))
	{
		BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	}

private:
	Scoped_context _previous;
};
