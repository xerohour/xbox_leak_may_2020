/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as	agreed in writing with
 *  Dinkumware, Ltd.
 */

#ifndef ITER_H
#define ITER_H
#include <iterator>
		// CONSTANTS
const int bad_copy = 0x01;
const int bad_compare = 0x02;
const int bad_indirect = 0x04;
const int bad_preinc = 0x08;
const int bad_postinc = 0x10;
const int bad_assign = 0x40;
const int bad_destroy = 0x80;
const int bad_predec = 0x100;
const int bad_postdec = 0x200;
const int bad_incn = 0x400;
		// TEMPLATE CLASS strict_random_access_iterator
template<class Iter, class T>
	class strict_random_access_iterator
		: public iterator<random_access_iterator_tag, T, int,
			T *, T&> {
public:
	strict_random_access_iterator()
		: iter(Iter()), perr(&recno), pcnt(&recno),
			maxrec(0), recno(0)
		{}
	strict_random_access_iterator(Iter it, int& err,
		int& cnt, int off = 0)
		: iter(it), perr(&err), pcnt(&cnt), recno(cnt - off),
			maxrec(cnt)
		{*perr = 0; }
	bool operator==(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter == x.iter); }
	bool operator!=(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (!(*this == x)); }
	T& operator*() const
		{if (recno <= 0 || maxrec < recno)
			*perr |= bad_indirect;
		return (*iter); }
#if _HAS_TOLERANT_POINTER_OPERATOR
	T *operator->() const
		{return (&**this); }
#endif
	strict_random_access_iterator<Iter, T>& operator++()
		{if (recno <= 0)
			*perr |= bad_preinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (*this); }
	strict_random_access_iterator<Iter, T> operator++(int)
		{strict_random_access_iterator<Iter, T> tmp(*this);
		if (recno <= 0)
			*perr |= bad_postinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (tmp); }
	strict_random_access_iterator<Iter, T>& operator--()
		{if (maxrec <= recno)
			*perr |= bad_predec;
		else
			++recno;
		--iter;
		return (*this); }
	strict_random_access_iterator<Iter, T> operator--(int)
		{strict_random_access_iterator<Iter, T> tmp(*this);
		if (recno == 0)
			*perr |= bad_postdec;
		else
			++recno;
		--iter;
		return (tmp); }
	strict_random_access_iterator<Iter, T>& operator+=(int n)
		{if (0 < n && recno < n || n < 0 && n < recno - maxrec)
			*perr |= bad_incn;
		else
			recno -= n;
		if (recno < *pcnt)
			*pcnt = recno;
		iter += n;
		return (*this); }
	strict_random_access_iterator<Iter, T>& operator-=(int n)
		{return (*this += -n); }
	int operator-(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter - x.iter); }
	T& operator[](int n) const
		{strict_random_access_iterator<Iter, T> tmp = *this;
		tmp += n;
		return (*tmp); }
	bool operator<(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter < x.iter); }
	bool operator>(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter > x.iter); }
	bool operator<=(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter <= x.iter); }
	bool operator>=(
		const strict_random_access_iterator<Iter, T> &x) const
		{return (iter >= x.iter); }
private:
	int *perr, *pcnt, recno, maxrec;
	Iter iter;
	};
template<class Iter, class T>
	strict_random_access_iterator<Iter, T>
		operator+(const strict_random_access_iterator<Iter, T> x,
			int n)
	{strict_random_access_iterator<Iter, T> tmp = x;
	return (tmp += n); }
template<class Iter, class T>
	strict_random_access_iterator<Iter, T>
		operator+(int n,
			const strict_random_access_iterator<Iter, T> x)
	{strict_random_access_iterator<Iter, T> tmp = x;
	return (tmp += n); }
template<class Iter, class T>
	strict_random_access_iterator<Iter, T>
		operator-(const strict_random_access_iterator<Iter, T> x,
			int n)
	{strict_random_access_iterator<Iter, T> tmp = x;
	return (tmp -= n); }
template<class Iter, class T>
	strict_random_access_iterator<Iter, T>
		operator-(int n,
			const strict_random_access_iterator<Iter, T> x)
	{strict_random_access_iterator<Iter, T> tmp = x;
	return (tmp -= n); }
		// TEMPLATE CLASS strict_bidirectional_iterator
template<class Iter, class T>
	class strict_bidirectional_iterator
		: public iterator<bidirectional_iterator_tag, T, int,
			T *, T&> {
public:
	strict_bidirectional_iterator()
		: iter(Iter()), perr(&recno), pcnt(&recno),
			maxrec(0), recno(0)
		{}
	strict_bidirectional_iterator(Iter it, int& err,
		int& cnt, int off = 0)
		: iter(it), perr(&err), pcnt(&cnt),
			recno(cnt - off), maxrec(cnt)
		{*perr = 0; }
	bool operator==(
		const strict_bidirectional_iterator<Iter, T> &X) const
		{return (iter == X.iter); }
	bool operator!=(
		const strict_bidirectional_iterator<Iter, T> &X) const
		{return (!(*this == X)); }
	T& operator*() const
		{if (recno <= 0 || maxrec < recno)
			*perr |= bad_indirect;
		return (*iter); }
#if _HAS_TOLERANT_POINTER_OPERATOR
	T *operator->() const
		{return (&**this); }
#endif
	strict_bidirectional_iterator<Iter, T>& operator++()
		{if (recno <= 0)
			*perr |= bad_preinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (*this); }
	strict_bidirectional_iterator<Iter, T> operator++(int)
		{strict_bidirectional_iterator<Iter, T> tmp(*this);
		if (recno <= 0)
			*perr |= bad_postinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (tmp); }
	strict_bidirectional_iterator<Iter, T>& operator--()
		{if (maxrec <= recno)
			*perr |= bad_predec;
		else
			++recno;
		--iter;
		return (*this); }
	strict_bidirectional_iterator<Iter, T> operator--(int)
		{strict_bidirectional_iterator<Iter, T> tmp(*this);
		if (recno == 0)
			*perr |= bad_postdec;
		else
			++recno;
		--iter;
		return (tmp); }
private:
	int *perr, *pcnt, recno, maxrec;
	Iter iter;
	};
		// TEMPLATE CLASS strict_forward_iterator
template<class Iter, class T>
	class strict_forward_iterator
		: public iterator<forward_iterator_tag, T, int,
			T *, T&> {
public:
	strict_forward_iterator()
		: iter(Iter()), perr(&recno), pcnt(&recno), recno(0)
		{}
	strict_forward_iterator(Iter it, int& err,
		int& cnt, int off = 0)
		: iter(it), perr(&err), pcnt(&cnt), recno(cnt - off)
		{*perr = 0; }
	bool operator==(
		const strict_forward_iterator<Iter, T> &X) const
		{return (iter == X.iter); }
	bool operator!=(
		const strict_forward_iterator<Iter, T> &X) const
		{return (!(*this == X)); }
	T& operator*() const
		{if (recno <= 0)
			*perr |= bad_indirect;
		return (*iter); }
#if _HAS_TOLERANT_POINTER_OPERATOR
	T *operator->() const
		{return (&**this); }
#endif
	strict_forward_iterator<Iter, T>& operator++()
		{if (recno <= 0)
			*perr |= bad_preinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (*this); }
	strict_forward_iterator<Iter, T> operator++(int)
		{strict_forward_iterator<Iter, T> tmp(*this);
		if (recno <= 0)
			*perr |= bad_postinc;
		else
			--recno;
		if (recno < *pcnt)
			*pcnt = recno;
		++iter;
		return (tmp); }
private:
	int *perr, *pcnt, recno;
	Iter iter;
	};
		// TEMPLATE CLASS strict_input_iterator
template<class Iter, class T>
	class strict_input_iterator
		: public iterator<input_iterator_tag, T, int,
			T *, T&> {
public:
	strict_input_iterator(Iter it, int& err,
		int& cnt, int off = 0)
		: iter(it), perr(&err), pcnt(&cnt), recno(cnt - off)
		{*perr = 0; }
	bool operator==(
		const strict_input_iterator<Iter, T> &X) const
		{return (iter == X.iter); }
	bool operator!=(
		const strict_input_iterator<Iter, T> &X) const
		{return (!(*this == X)); }
	T operator*() const
		{if (recno <= 0)
			*perr |= bad_indirect;
		return (*iter); }
#if _HAS_TOLERANT_POINTER_OPERATOR
	T *operator->() const
		{return (&**this); }
#endif
	strict_input_iterator<Iter, T>& operator++()
		{if (*pcnt != recno)
			*perr |= bad_preinc;
		else if (0 < *pcnt)
			recno = --*pcnt;
		++iter;
		return (*this); }
	strict_input_iterator<Iter, T> operator++(int)
		{strict_input_iterator<Iter, T> tmp(*this);
		if (*pcnt != recno)
			*perr |= bad_postinc;
		else if (0 < *pcnt)
			recno = --*pcnt;
		++iter;
		return (tmp); }
private:
	int *perr, *pcnt, recno;
	Iter iter;
	};
		// TEMPLATE CLASS strict_output_iterator
template<class Iter, class T>
	class strict_output_iterator
		: public iterator<output_iterator_tag, void, void,
			void, void> {
public:
	enum State {ready = 0, postinc = 1, indirp = 2,
		indir = 4, assigned = 8, dead = 16};
	strict_output_iterator(Iter it, int& err,
		int& cnt, int off = 0)
		: iter(it), piter(&iter), perr(&err), pcnt(&cnt),
			recno(cnt - off), state(ready)
		{*perr = 0; }
	strict_output_iterator(
		const strict_output_iterator<Iter, T>& X)
		: piter(X.piter), perr(X.perr), pcnt(X.pcnt),
			recno(X.recno), state(X.state)
		{if (state != ready && state != postinc)
			*perr |= bad_copy; }
	void operator=(
		const strict_output_iterator<Iter, T> &X)
		{piter = X.piter, perr = X.perr, pcnt = X.pcnt,
			recno = X.recno, state = X.state;
		if (state != ready)
			*perr |= bad_copy; }
//	~strict_output_iterator()
//		{if (state != ready)
//			*perr |= bad_destroy; }
	strict_output_iterator<Iter, T>& operator=(const T& V)
		{if (state == indirp)
			state = dead, *pcnt = recno;
		else if (state == indir)
			state = assigned;
		else
			*perr |= bad_assign, state = ready;
		**piter = V;
		++*piter;
		return (*this); }
	strict_output_iterator<Iter, T>& operator*()
		{if (state == postinc)
			state = indirp;
		else if (state == ready)
			state = indir;
		else
			*perr |= bad_indirect, state = indir;
		return (*this); }
	strict_output_iterator<Iter, T>& operator++()
		{if (state != assigned)
			*perr |= bad_preinc;
		state = ready;
		if (*pcnt == 0 || *pcnt != recno)
			*perr |= bad_preinc;
		if (0 < *pcnt)
			recno = --*pcnt;
		return (*this); }
	strict_output_iterator<Iter, T> operator++(int)
		{strict_output_iterator<Iter, T> tmp(*this);
		tmp.state = postinc;
		if (state != ready || *pcnt == 0 || *pcnt != recno)
			*perr |= bad_postinc;
		if (0 < *pcnt)
			recno = *pcnt - 1;
		tmp.recno = recno, *pcnt = 0;
		return (tmp); }
private:
	Iter iter;	// full constructor only
	int *perr, *pcnt, recno;
	Iter *piter;
	State state;
	};
#endif  /* of ITER_H */
