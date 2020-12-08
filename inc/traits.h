#pragma once

namespace {

  struct _ {
    bool operator == (struct _) const { return true; }
  };

  template <typename R, typename I, typename A, typename B, typename C, typename D>
  struct detail {
    typedef R RT;
    typedef I IF;
    typedef A A0;
    typedef B A1;
    typedef C A2;
    typedef D A3;
  };

  template <typename T> struct traits;

  template <typename R>
  struct traits<R (*)()> : detail<R,_,_,_,_,_> {
    enum { Arguments = 0 };
    typedef R(*SG)();
  };

  template <typename R, typename A>
  struct traits<R (*)(A)> : detail<R,_,A,_,_,_> { 
    enum { Arguments = 1 };
    typedef R(*SG)(A);
  };

  template <typename R, typename A, typename B>
  struct traits<R (*)(A,B)> : detail<R,_,A,B,_,_> { 
    enum { Arguments = 2 };
    typedef R(*SG)(A,B);
  };

  template <typename R, typename A, typename B, typename C>
  struct traits<R (*)(A,B,C)> : detail<R,_,A,B,C,_> { 
    enum { Arguments = 3 };
    typedef R(*SG)(A,B,C);
  };

  template <typename R, typename A, typename B, typename C, typename D>
  struct traits<R (*)(A,B,C,D)> : detail<R,_,A,B,C,D> { 
    enum { Arguments = 4 };
    typedef R(*SG)(A,B,C,D);
  };

  template <typename R, typename I>
  struct traits<R (I::*)()> : detail<R,I,_,_,_,_> {
    enum { Arguments = 0 };
  };

  template <typename R, typename I, typename A>
  struct traits<R (I::*)(A)> : detail<R,I,A,_,_,_> { 
    enum { Arguments = 1 };
  };

  template <typename R, typename I, typename A, typename B>
  struct traits<R (I::*)(A,B)> : detail<R,I,A,B,_,_> { 
    enum { Arguments = 2 };
  };

  template <typename R, typename I, typename A, typename B, typename C>
  struct traits<R (I::*)(A,B,C)> : detail<R,I,A,B,C,_> { 
    enum { Arguments = 3 };
  };

  template <typename R, typename I, typename A, typename B, typename C, typename D>
  struct traits<R (I::*)(A,B,C,D)> : detail<R,I,A,B,C,D> { 
    enum { Arguments = 4 };
  };

  template <typename M, typename S> static M unsafe_cast(S source) {
    union { S source; M result; } cast;
    cast.result = static_cast<M>(0);
    cast.source = source;
    return cast.result;
  }

}