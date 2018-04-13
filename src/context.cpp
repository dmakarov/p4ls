#include "context.h"

static Context &create_empty_context() {
	thread_local static auto context = Context::get_empty();
	return context;
}

Context Context::get_empty()
{
	return Context(nullptr);
}

const Context &Context::get_current()
{
	return create_empty_context();
}

Context Context::swap_current(Context other)
{
	std::swap(other, create_empty_context());
	return other;
}
