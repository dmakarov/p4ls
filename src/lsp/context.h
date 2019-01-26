/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/log/common.hpp>
#include <boost/log/sinks/syslog_backend.hpp>

#include <memory>
#include <ostream>
#include <sstream>
#include <type_traits>

template <class T> class Key {
public:
	static_assert(!std::is_reference<T>::value, "");
	Key() = default;
	Key(const Key&) = delete;
	Key(Key&&) = delete;
	Key& operator=(const Key&) = delete;
	Key& operator=(Key&&) = delete;
};

class Context {
	struct Data;

	friend std::ostream& operator<<(std::ostream& os, const Context& context);
	friend std::ostream& operator<<(std::ostream& os, const Context::Data& data);

public:
	static Context create_empty();
	static const Context& get_current();
	static Context swap_current(Context other);
	static boost::log::sources::severity_logger<int> _logger;

	Context() = default;
	Context(const Context&) = delete;
	Context(Context&&) = default;
	Context& operator=(const Context&) = delete;
	Context& operator=(Context&&) = default;

	template <class T> const T* get_value(const Key<T>& key) const
	{
		for (auto* it = this->_data.get(); it != nullptr; it = it->_parent.get())
		{
			if (it->_key == &key)
			{
				return static_cast<const T*>(it->_value->get_value());
			}
		}
		BOOST_LOG(_logger) << "did not find a value for key \"" << &key << "\"";
		return nullptr;
	}

	template <class T> const T& get_existing(const Key<T>& key) const
	{
		auto value = get_value(key);
		assert(value && "key does not exist");
		return *value;
	}

	template <class T> Context derive(const Key<T>& key, typename std::decay<T>::type value) const &
	{
		return Context(std::make_shared<Data>(Data{_data, std::make_unique<Something<typename std::decay<T>::type>>(std::move(value)), &key}));
	}

	template <class T> Context derive(const Key<T>& key, typename std::decay<T>::type value) &&
	{
		return Context(std::make_shared<Data>(Data{std::move(_data), std::make_unique<Something<typename std::decay<T>::type>>(std::move(value)), &key}));
	}

	template <class T> Context derive(T&& value) const &
	{
		static Key<typename std::decay<T>::type> _private_key;
		return derive(_private_key, std::forward<T>(value));
	}

	template <class T> Context derive(T&& value) &&
	{
		static Key<typename std::decay<T>::type> _private_key;
		return std::move(this)->derive(_private_key, std::forward<T>(value));
	}

	Context clone() const
	{
		return Context(_data);
	}

private:
	class Anything {
	public:
		virtual ~Anything() = default;
		virtual void* get_value() = 0;
		virtual std::string to_string() = 0;
	};

	template <class T> class Something : public Anything {
		static_assert(std::is_same<typename std::decay<T>::type, T>::value, "");
	public:

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
		Something(T&& value) : _value(std::move(value)) {}
#pragma GCC diagnostic pop

		void* get_value() override
		{
			return &_value;
		}

		std::string to_string() override
		{
			std::stringstream ss;
			ss << _value;
			return ss.str();
		}

	private:
		T _value;
	};

	struct Data {
		// _parent must be destroyed last after destruction of _value.
		// objects stored in Context's child layers may store
		// references to the data in the parent layers.
		std::shared_ptr<const Data> _parent;
		std::unique_ptr<Anything> _value;
		const void* _key;
	};

	std::shared_ptr<const Data> _data;

	Context(std::shared_ptr<const Data> data) : _data(std::move(data))
	{
		BOOST_LOG(_logger) << "constructed, current " << *this;
	}

};

class Scoped_context {
public:
	template <typename T> Scoped_context(const Key<T>& key, typename std::decay<T>::type value)
		: _previous(Context::swap_current(std::move(Context::get_current().derive(key, std::move(value)))))
	{}

	// Anonymous values can be used for the destructor side-effect.
	template <typename T> Scoped_context(T&& value)
		: _previous(Context::swap_current(std::move(Context::get_current().derive(std::forward<T>(value)))))
	{}

	~Scoped_context()
	{
		Context::swap_current(std::move(_previous));
		BOOST_LOG(Context::_logger)
			<< "destroyed, current " << Context::get_current();

	}
	Scoped_context(const Scoped_context&) = delete;
	Scoped_context(Scoped_context&&) = delete;
	Scoped_context& operator=(const Scoped_context&) = delete;
	Scoped_context& operator=(Scoped_context&&) = delete;

private:
	Context _previous;
};

std::ostream& operator<<(std::ostream& os, const Context::Data& data);
std::ostream& operator<<(std::ostream& os, const Context& context);
