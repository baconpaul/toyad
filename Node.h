#ifndef _INCLUDE_NODE_H
#define _INCLUDE_NODE_H

#include <iomanip>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <math.h>

static std::string nextsym( const char* in );

class Node
{
public:
  typedef std::shared_ptr< Node > ptr_t;
  typedef std::map< std::string, double > context_t;
  typedef std::vector< ptr_t > children_t;
  typedef double value_t;

protected:
  std::string _name;

  value_t     _value;
  value_t     _dvalue;

  bool        _vstale;

  virtual ~Node() {}
  
  Node( const char* pfx )
  {
    _name = nextsym( pfx );
    _value = 0;
    _dvalue = 0;

    _vstale  = true;
  }

public:
  virtual void fwd_eval( const context_t &ctx ) = 0;
  virtual void bkw_deriv_eval( const value_t &d ) = 0;
  
  virtual std::string name() const { return _name; }
  virtual value_t get()
  {
    return _value;
  }
  virtual value_t getderiv()
  {
    return _dvalue;
  }

  virtual std::ostream& print( std::ostream &os ) const = 0;
  virtual children_t children() { return children_t(); }

  virtual void invalidate()
  {
    if( ! _vstale )
      {
	_vstale = true;
	children_t kids = children();
	for( auto it = kids.begin(); it<kids.end(); ++it )
	  (*it)->invalidate();
      }
  }
  
};

std::ostream& operator<<( std::ostream &out, const Node &n )
{
  return n.print( out );
}

std::ostream& operator<<( std::ostream &out, const Node::ptr_t &n )
{
  return n->print( out );
}


class Constant : public Node {
private:
  Constant( const value_t & v ) : Node( "const" )
  {
    _value = v;
  }
  
public:
  static ptr_t make( const value_t & val ) 
  {
    return ptr_t( new Constant( val ) );
  }

  virtual void fwd_eval( const context_t & ) {
    _vstale = false;
  }
  virtual void bkw_deriv_eval( const value_t & ) {
  }

  virtual std::ostream& print( std::ostream &os ) const { os << _value; return os; }
};

class Variable : public Node {
private:

  Variable( const std::string & n ) :  Node( "var" )
  {
    _name = n;
  }
  
public:
  static ptr_t make( const std::string & name ) 
  {
    return ptr_t( new Variable( name ) );
  }

  virtual void fwd_eval( const context_t &ctx )
  {
    if( _vstale )
      {
	context_t::const_iterator p = ctx.find( _name );
	if( p != ctx.end() )
	  {
	    _value  = p->second;
	  }
	_vstale = false;
      }
  }

  virtual void bkw_deriv_eval( const value_t &t )
  {
    _dvalue += t;
  }

  virtual std::ostream& print( std::ostream &os ) const { os << _name; return os; }
};

class BinOp : public Node
{
 protected:
  typedef std::function< value_t( value_t, value_t) > func_t;
  // For now
  ptr_t _a, _b;
  children_t _children;
  func_t _bop;
  std::string _opn;
  
 BinOp( ptr_t a,
	ptr_t b,
	func_t bop,
	const char* opn,
	const char* pfx )
   : Node( pfx ),
    _a(a), _b(b), _children(), _bop( bop ),
    _opn( opn )
    
    {
      _children.push_back( _a );
      _children.push_back( _b );
    }
  
public:

  virtual void fwd_eval( const context_t &ctx )
  {
    if( _vstale )
      {
	_a->fwd_eval( ctx );
	_b->fwd_eval( ctx );
	_value = _bop( _a->get(), _b->get() );
	_vstale = false;
      }
  }

  virtual std::ostream& print( std::ostream &os ) const {
    os << "( " << _a << " " << _opn << " " << _b << " )";
    return os;
  }

  virtual children_t children() { return _children; }

};

class Plus : public BinOp
{
protected:
 Plus( ptr_t a, ptr_t b ) : BinOp( a, b, [](value_t xx, value_t yy) { return xx + yy; }, "+", "plus" ) {
  }

public:
  static ptr_t make( ptr_t a, ptr_t b ) {
    return ptr_t( new Plus( a, b ) );
  }

  virtual void bkw_deriv_eval( const value_t &t )
  {
    // f = q = x + y
    //
    // so if df/dq = t
    // then df/dx = df/dq dq/dy = df/dq
    // and  df/dy = df/dq dq/dy = df/dq
    _dvalue += t;
    _a->bkw_deriv_eval( t );
    _b->bkw_deriv_eval( t );
  }
};

class Minus : public BinOp
{
protected:
 Minus( ptr_t a, ptr_t b ) : BinOp( a, b, []( value_t xx, value_t yy ) { return xx - yy; }, "-", "minus" ) {}

public:
  static ptr_t make( ptr_t a, ptr_t b ) {
    return ptr_t( new Minus( a, b ) );
  }

  virtual void bkw_deriv_eval( const value_t &t )
  {
    // f = q = x - y
    //
    // so if df/dq = t
    // then df/dx = df/dq dq/dx = df/dq
    // and  df/dy = df/dq dq/dy = -df/dq
    _dvalue += t;
    _a->bkw_deriv_eval( t );
    _b->bkw_deriv_eval( -t );
  }

};

class Mul : public BinOp
{
protected:
 Mul( ptr_t a, ptr_t b ) : BinOp( a, b, []( value_t xx, value_t yy ) { return xx * yy; }, "*", "mul" ) {}


public:
  static ptr_t make( ptr_t a, ptr_t b ) {
    return ptr_t( new Mul( a, b ) );
  }
  
  virtual value_t bop( value_t a, value_t b ) { return a * b; }
  
  virtual void bkw_deriv_eval( const value_t &t )
  {
    // f = q = a * b
    //
    // so if df/dq = t
    // then df/da = df/dq dq/da = df/dq * b
    // and  df/db = df/dq dq/db = df/dq * a
    _dvalue += t;
    _a->bkw_deriv_eval( t * _b->get() );
    _b->bkw_deriv_eval( t * _a->get() );
  }
  virtual std::string getopstr() const { return "*"; }
};

class Div : public BinOp
{
 protected:
 Div( ptr_t a, ptr_t b ) : BinOp( a, b, []( value_t xx, value_t yy ) { return xx / yy; }, "/", "div" ) {}
  
 public:
  static ptr_t make( ptr_t a, ptr_t b ) {
    return ptr_t( new Div( a, b ) );
  }
  virtual value_t bop( value_t a, value_t b ) { return a / b; }
  
  virtual void bkw_deriv_eval( const value_t &t )
  {
    // f = q = a / b
    //
    // so if df/dq = t
    // then df/da = df/dq dq/da = df/dq / b
    // and  df/db = df/dq dq/db = df/dq * ( - a / b^2 )
    _dvalue += t;
    _a->bkw_deriv_eval( t / _b->get() );
    _b->bkw_deriv_eval( - t * _a->get() / ( _b->get() * _b->get() ));
  }
  virtual std::string getopstr() const { return "/"; }
};

// const V^const
class WeightedPoly : public Node
{
protected:
  value_t _a;
  ptr_t _x;
  int _m;
  children_t _children;
  // a x ^ m
 WeightedPoly( value_t a, ptr_t x, int m ) : Node( "wpoly" ), _a(a), _x(x), _m(m), _children() 
  {
    _children.push_back( _x );
  }
public:
  static ptr_t make( value_t a, ptr_t x, int m )
  {
    return ptr_t( new WeightedPoly( a, x, m ) );
  }

  virtual void fwd_eval( const context_t &ctx )
  {
    if( _vstale )
      {
	_x->fwd_eval( ctx );
	_value = _a * pow( _x->get(), _m );
	_vstale = false;
      }
  }

  virtual void bkw_deriv_eval( const value_t &t )
  {
    // f = q = a x ^ m
    //
    // so if df/dq = t
    // then df/dx = df/dq dq/dx = df/dq (a * m * x^(m-1) )
    _dvalue += t;
    _x->bkw_deriv_eval( t * _a * _m * pow( _x->get(), _m - 1 ) );
  }
  
  virtual std::ostream& print( std::ostream &os ) const
  {
    os << _a << " " << _x << "^" << _m;
    return os;
  }

  virtual children_t children() { return _children; }

};

#endif
