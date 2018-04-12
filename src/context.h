/*
 * -*- c++ -*-
 */

#pragma once

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

	Context(std::shared_ptr<const Data> data) : _data(std::move(data)) {}

public:

	static Context get_empty();
	static const Context &get_current();
	static Context swap_current(Context other);

	Context() = default;
	Context(Context const &) = delete;
	Context(Context &&) = default;
	Context &operator=(Context const &) = delete;
	Context &operator=(Context &&) = default;

	template <class T> const T *get_value(const Key<T> &key) const
	{
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
		auto value = get_value(key);
		assert(value && "key does not exist");
		return *value;
	}

	template <class T> Context derive(const Key<T> &key, typename std::decay<T>::type value) const &
	{
		return Context(std::make_shared<Data>(Data{_data, &key, std::make_unique<Something<typename std::decay<T>::type>>(std::move(value))}));
	}

	template <class T> Context derive(const Key<T> &key, typename std::decay<T>::type value) &&
	{
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
		return std::move(this)->derive(_private_key, std::forward<T>(value));
	}

	Context clone() const
	{
		return Context(_data);
	}

};

class Scoped_context {
public:
	Scoped_context(Context context) : previous(Context::swap_current(std::move(context))) {}
	~Scoped_context()
	{
		Context::swap_current(std::move(previous));
	}
	Scoped_context(Scoped_context const &) = delete;
	Scoped_context(Scoped_context &&) = delete;
	Scoped_context &operator=(Scoped_context const &) = delete;
	Scoped_context &operator=(Scoped_context &&) = delete;

private:
	Context previous;
};

class Scoped_context_with_value {
public:
	template <typename T> Scoped_context_with_value(const Key<T> &key, typename std::decay<T>::type value)
		: previous(Context::get_current().derive(key, std::move(value))) {}

	// Anonymous values can be used for the destructor side-effect.
	template <typename T> Scoped_context_with_value(T &&value)
		: previous(Context::get_current().derive(std::forward<T>(value))) {}

private:
	Scoped_context previous;
};
