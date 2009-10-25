// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/

#ifndef TIARY_COMMON_ITERATOR_H
#define TIARY_COMMON_ITERATOR_H

#include <iterator>
#include <functional>

namespace tiary {

template <typename Operator>
	struct UnaryOperatorTraits
{
	typedef typename Operator::result_type result_type;
	typedef typename Operator::argument_type argument_type;
};

template <typename Result, typename Argument>
	struct UnaryOperatorTraits <Result (*)(Argument)>
{
	typedef Result result_type;
	typedef Argument argument_type;
};

template <typename Result, typename Argument>
	struct UnaryOperatorTraits <Result (&)(Argument)>
{
	typedef Result result_type;
	typedef Argument argument_type;
};

template <typename Operator, typename Iterator>
	class TransformIterator : public std::input_iterator_tag
{
	Operator op;
	Iterator it;

public:
	typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
	typedef typename UnaryOperatorTraits<Operator>::result_type value_type;
	typedef void pointer;
	typedef void reference;
	typedef std::input_iterator_tag iterator_category;

	TransformIterator (Operator op_, Iterator it_) : op (op_), it (it_) {}
	TransformIterator (const TransformIterator &other) : op (other.op), it (other.it) {}
	TransformIterator &operator = (const TransformIterator &other)
	{
		op = other.op;
		it = other.it;
		return *this;
	}

	bool operator == (const TransformIterator &other) const
	{
		return (it == other.it);
	}
	bool operator != (const TransformIterator &other) const
	{
		return !(*this == other);
	}

	value_type operator * () const { return op (*it); }
	value_type operator -> () const { return op (*it); }

	TransformIterator &operator ++ ()
	{
		++it;
		return *this;
	}
	TransformIterator operator ++ (int)
	{
		TransformIterator copy (*this);
		++it;
		return copy;
	}
};

template <typename Operator, typename Iterator> inline
TransformIterator <Operator, Iterator> transform_iterator (Operator op, Iterator it)
{
	return TransformIterator <Operator, Iterator> (op, it);
}

} // namespace tiary

#endif // include guard
