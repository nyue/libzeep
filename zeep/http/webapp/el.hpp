// Copyright Maarten L. Hekkelman, Radboud University 2008-2011.
//   Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//            http://www.boost.org/LICENSE_1_0.txt)
//
// expression language support
//

#pragma once

#include <map>

#include <boost/range.hpp>
#include <boost/cstdint.hpp>

#include <zeep/http/request.hpp>
#include <zeep/exception.hpp>

#ifndef LIBZEEP_DOXYGEN_INVOKED
typedef boost::int8_t		int8;
typedef boost::uint8_t		uint8;
typedef boost::int16_t		int16;
typedef boost::uint16_t		uint16;
typedef boost::int32_t		int32;
typedef boost::uint32_t		uint32;
typedef boost::int64_t		int64;
typedef boost::uint64_t		uint64;
#endif

namespace zeep {
namespace http {
namespace el
{

namespace detail
{
class object_impl;
class object_iterator_impl;
};

class scope;

/// This zeep::http::el::object class is a bridge to the `el` expression language.

class object
{
  public:

	/// object can have one of these basic types:
	enum object_type
	{
		null_type,
		array_type,
		struct_type,
		number_type,
		string_type
	};

				object();
				object(const object& o);
	explicit	object(detail::object_impl* impl);
				~object();
				
				/// create an array object
	explicit	object(const std::vector<object>& v);
	explicit	object(const std::vector<std::string>& v);

				/// construct an object directly from some basic types
	explicit	object(bool v);
	explicit	object(int8 v);
	explicit	object(uint8 v);
	explicit	object(int16 v);
	explicit	object(uint16 v);
	explicit	object(int32 v);
	explicit	object(uint32 v);
	explicit	object(int64 v);
	explicit	object(uint64 v);
	explicit	object(float v);
	explicit	object(double v);
	explicit	object(const char* v);
	explicit	object(const std::string& v);

	object&		operator=(const object& o);

				/// assign an array object
	object&		operator=(const std::vector<object>& v);
	object&		operator=(const std::vector<std::string>& v);

				/// and assign some basic types
	object&		operator=(bool v);
	object&		operator=(int8 v);
	object&		operator=(uint8 v);
	object&		operator=(int16 v);
	object&		operator=(uint16 v);
	object&		operator=(int32 v);
	object&		operator=(uint32 v);
	object&		operator=(int64 v);
	object&		operator=(uint64 v);
	object&		operator=(float v);
	object&		operator=(double v);
	object&		operator=(const char* v);
	object&		operator=(const std::string& v);

	object_type	type() const;

	template<typename T>
	T			as() const;

	const object operator[](const std::string& name) const;
	const object operator[](const char* name) const;
	const object operator[](const object& index) const;

	object&		operator[](const std::string& name);
	object&		operator[](const char* name);
	object&		operator[](const object& index);
	
	size_t		count() const;
	bool		empty() const;

	bool		operator<(const object& rhs) const;
	bool		operator==(const object& rhs) const;

	template<class ObjectType>
	class basic_iterator : public std::iterator<std::forward_iterator_tag, ObjectType>
	{
	  public:
		typedef typename std::iterator<std::forward_iterator_tag, ObjectType>	base_type;
		typedef typename base_type::reference									reference;
		typedef typename base_type::pointer										pointer;

						basic_iterator();
						basic_iterator(const basic_iterator& other);
						basic_iterator(detail::object_impl* a);
						basic_iterator(detail::object_impl* a, int);
						basic_iterator(const detail::object_impl* a);
						basic_iterator(const detail::object_impl* a, int);
						~basic_iterator();
		
		basic_iterator&	operator=(const basic_iterator& other);
		
		reference		operator*() const;
		pointer			operator->() const;

		basic_iterator&	operator++();
		basic_iterator	operator++(int);

		bool			operator==(const basic_iterator& other) const;
		bool			operator!=(const basic_iterator& other) const;

	  private:
		detail::object_iterator_impl*
						m_impl;
	};

	typedef basic_iterator<object>			iterator;
	typedef basic_iterator<const object>	const_iterator;

	iterator			begin()								{ return iterator(m_impl); }
	iterator			end()								{ return iterator(m_impl, -1); }

	const_iterator		begin() const						{ return const_iterator(m_impl); }
	const_iterator		end() const							{ return const_iterator(m_impl, -1); }

	friend iterator			range_begin(object& x)			{ return x.begin(); }
	friend iterator			range_end(object& x)			{ return x.end(); }
	friend const_iterator	range_begin(const object& x)	{ return x.begin(); }
	friend const_iterator	range_end(const object& x)		{ return x.end(); }

	friend std::ostream& operator<<(std::ostream& lhs, const object& rhs);
	
	std::string			toJSON() const;

  private:

	friend object operator+(const object& a, const object& b);
	friend object operator-(const object& a, const object& b);
	friend object operator*(const object& a, const object& b);
	friend object operator%(const object& a, const object& b);
	friend object operator/(const object& a, const object& b);
	friend object operator-(const object& a);

	class detail::object_impl*	m_impl;
};

/// \brief Process the text in \a text and return `true` if the result is
///        not empty, zero or false.
///
///	The expression in \a text is processed and if the result of this
/// expression is empty, false or zero then `false` is returned.
/// \param scope  The scope for this el script
/// \param text   The el script
/// \return       The result of the script
bool process_el(const scope& scope, std::string& text);

/// \brief Process the text in \a text. The result is put in \a result
///
///	The expression in \a text is processed and the result is returned
/// in \a result.
/// \param scope  The scope for this el script
/// \param text   The el script
/// \return       The result of the script
void evaluate_el(const scope& scope, const std::string& text, object& result);

/// \brief Process the text in \a text and replace it with the result
///
///	The expressions found in \a text are processed and the output of
/// 				the processing is used as a replacement value for the expressions.
/// \param scope  The scope for the el scripts
/// \param text   The text optionally containing el scripts.
/// \return       Returns true if \a text was changed.
bool evaluate_el(const scope& scope, const std::string& text);

// --------------------------------------------------------------------

class scope
{
  public:
					scope(const request& req);
	explicit		scope(const scope& next);

	template<typename T>
	void			put(const std::string& name, const T& value);

	template<typename ForwardIterator>
	void			put(const std::string& name, ForwardIterator begin, ForwardIterator end);

	const object&	lookup(const std::string& name) const;
	const object&	operator[](const std::string& name) const;

	object&			lookup(const std::string& name);
	object&			operator[](const std::string& name);

	const request&	get_request() const;

  private:

	friend std::ostream& operator<<(std::ostream& lhs, const scope& rhs);

	scope&			operator=(const scope&);

	typedef std::map<std::string,object> data_map;

	data_map		m_data;
	scope*			m_next;
	const request*	m_req;
};

/// for debugging purposes
std::ostream& operator<<(std::ostream& lhs, const scope& rhs);

template<typename T>
inline
void scope::put(
	const std::string&	name,
	const T&		value)
{
	m_data[name] = object(value);
}

template<>
inline
void scope::put(
	const std::string&	name,
	const object&	value)
{
	m_data[name] = value;
}

template<typename ForwardIterator>
inline
void scope::put(
	const std::string&	name,
	ForwardIterator	begin,
	ForwardIterator	end)
{
	std::vector<object> elements;
	while (begin != end)
		elements.push_back(object(*begin++));
	m_data[name] = object(elements);
}

// --------------------------------------------------------------------

namespace detail
{

class object_iterator_impl;

class object_impl
{
  public:

	void					reference()						{ ++m_refcount; }
	void					release()						{ if (--m_refcount == 0) delete this; }

	object::object_type		type() const					{ return m_type; }
	bool					is_null() const					{ return m_type == object::null_type; }
	bool					is_array() const				{ return m_type == object::array_type; }
	bool					is_struct() const				{ return m_type == object::struct_type; }
	bool					is_number() const				{ return m_type == object::number_type; }
	bool					is_string() const				{ return m_type == object::string_type; }

	virtual void			print(std::ostream& os) const = 0;
	virtual int				compare(object_impl* rhs) const = 0;

	virtual int64			to_int() const;
	virtual double			to_double() const;
	virtual std::string		to_str() const;
	virtual std::string		to_JSON() const;

	friend class object;

  protected:

							object_impl(object::object_type type = object::null_type)
								: m_refcount(1)
								, m_type(type)
							{
							}

	virtual					~object_impl()
							{
							}

  private:
							object_impl(const object_impl&);
	object_impl&			operator=(const object_impl&);

	int32					m_refcount;
	object::object_type		m_type;
};

class base_array_object_impl : public object_impl
{
  public:

	virtual size_t			count() const = 0;

	virtual object_iterator_impl*
							create_iterator(bool begin) const = 0;

	virtual object&			at(uint32 ix);
	virtual const object	at(uint32 ix) const;

  protected:
							base_array_object_impl()
								: object_impl(object::array_type)
							{
							}
};

class base_struct_object_impl : public object_impl
{
  public:

	virtual object&			field(const std::string& name) = 0;
	virtual const object	field(const std::string& name) const = 0;

  protected:
							base_struct_object_impl()
								: object_impl(object::struct_type)
							{
							}
};

class object_iterator_impl
{
  public:

	void			reference()						{ ++m_refcount; }
	void			release()						{ if (--m_refcount == 0) delete this; }

					object_iterator_impl()
						: m_refcount(1)				{}
	
	virtual void	increment() = 0;
	virtual object&	dereference() = 0;
	virtual bool	equal(const object_iterator_impl* other) = 0;

  protected:
	virtual			~object_iterator_impl()			{}

  private:
	int				m_refcount;
};

}

// --------------------------------------------------------------------

template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator()
	: m_impl(nullptr)
{
}

template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator(const basic_iterator& o)
	: m_impl(o.m_impl)
{
	if (m_impl != nullptr)
		m_impl->reference();
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator(detail::object_impl* a)
	: m_impl(nullptr)
{
	detail::base_array_object_impl* impl = dynamic_cast<detail::base_array_object_impl*>(a);
	if (impl != nullptr)
		m_impl = impl->create_iterator(true);
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator(detail::object_impl* a, int)
	: m_impl(nullptr)
{
	detail::base_array_object_impl* impl = dynamic_cast<detail::base_array_object_impl*>(a);
	if (impl != nullptr)
		m_impl = impl->create_iterator(false);
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator(const detail::object_impl* a)
	: m_impl(nullptr)
{
	const detail::base_array_object_impl* impl = dynamic_cast<const detail::base_array_object_impl*>(a);
	if (impl != nullptr)
		m_impl = impl->create_iterator(true);
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>::basic_iterator(const detail::object_impl* a, int)
	: m_impl(nullptr)
{
	const detail::base_array_object_impl* impl = dynamic_cast<const detail::base_array_object_impl*>(a);
	if (impl != nullptr)
		m_impl = impl->create_iterator(false);
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>::~basic_iterator()
{
	if (m_impl != nullptr)
		m_impl->release();
}
		
template<class ObjectType>
object::basic_iterator<ObjectType>& object::basic_iterator<ObjectType>::operator=(const basic_iterator& o)
{
	if (this != &o)
	{
		if (m_impl != nullptr)
			m_impl->release();
		m_impl = o.m_impl;
		if (m_impl != nullptr)
			m_impl->reference();
	}
	return *this;
}
		
template<class ObjectType>
typename object::basic_iterator<ObjectType>::reference object::basic_iterator<ObjectType>::operator*() const
{
	if (m_impl == nullptr)
		throw exception("dereferencing invalid object iterator");
	return m_impl->dereference();
}

template<class ObjectType>
typename object::basic_iterator<ObjectType>::pointer object::basic_iterator<ObjectType>::operator->() const
{
	if (m_impl == nullptr)
		throw exception("dereferencing invalid object iterator");
	return &m_impl->dereference();
}

template<class ObjectType>
object::basic_iterator<ObjectType>& object::basic_iterator<ObjectType>::operator++()
{
	if (m_impl == nullptr)
		throw exception("incrementing invalid object iterator");
	m_impl->increment();
	return *this;
}

template<class ObjectType>
object::basic_iterator<ObjectType> object::basic_iterator<ObjectType>::operator++(int)
{
	if (m_impl == nullptr)
		throw exception("incrementing invalid object iterator");

	basic_iterator<ObjectType> iter(*this);
	m_impl->increment();
	return iter;
}

template<class ObjectType>
bool object::basic_iterator<ObjectType>::operator==(const basic_iterator& o) const
{
	bool result;
	if (m_impl == nullptr and o.m_impl == nullptr)
		result = true;
	else if (m_impl == nullptr or o.m_impl == nullptr)
		throw exception("invalid object iterators");
	else
		result = m_impl->equal(o.m_impl);
	return result;
}

template<class ObjectType>
bool object::basic_iterator<ObjectType>::operator!=(const basic_iterator& o) const
{
	bool result;
	if (m_impl == nullptr and o.m_impl == nullptr)
		result = false;
	else if (m_impl == nullptr or o.m_impl == nullptr)
		throw exception("invalid object iterators");
	else
		result = not m_impl->equal(o.m_impl);
	return result;
}


}
}
}

#ifndef LIBZEEP_DOXYGEN_INVOKED
// enable foreach (.., array object)
namespace boost
{
    // specialize range_mutable_iterator and range_const_iterator in namespace boost
    template<>
    struct range_mutable_iterator<zeep::http::el::object>
    {
        typedef zeep::http::el::object::iterator type;
    };

    template<>
    struct range_const_iterator<zeep::http::el::object>
    {
        typedef zeep::http::el::object::const_iterator type;
    };
}
#endif
