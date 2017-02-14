#include <iostream>
#include <iomanip>
#include "Node.h"

static int symct = 1;
std::string nextsym( const char* pfx )
{
  std::ostringstream ss;
  ss << pfx << "_" << symct++;
  return ss.str();
}


void super_simple()
{
  Node::ptr_t thr = Constant::make( 3 );
  Node::ptr_t x   = Variable::make( "x" );
  Node::ptr_t r   = Mul::make( x, thr );
  Node::context_t ctx;
  ctx[ x->name() ] = 3;
  r->fwd_eval( ctx );

  std::cout << "r := " << r << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  //r->bkw_deriv_eval( 1.0 );
  //std::cout << "and   df/dx = " << x->getderiv() << std::endl;
}

void xpx()
{
  Node::ptr_t x   = Variable::make( "x" );
  Node::ptr_t r   = Plus::make( x, x );
  Node::context_t ctx;
  ctx[ x->name() ] = 3;
  r->fwd_eval( ctx );

  std::cout << "r := " << r << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;

}

void spoly()
{
  Node::ptr_t x   = Variable::make( "x" );
  Node::ptr_t x2  = Mul::make( x, x );
  Node::ptr_t r   = Minus::make( x2, x );
  Node::context_t ctx;
  ctx[ x->name() ] = 3;
  r->fwd_eval( ctx );

  std::cout << "r := ";
  r->print( std::cout );
  std::cout << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;

}

void divsum()
{
  Node::ptr_t x   = Variable::make( "x" );
  Node::ptr_t c2  = Constant::make( 2 );
  Node::ptr_t xp2 = Plus::make( x, c2 );
  Node::ptr_t r   = Div::make( xp2, x );

  Node::context_t ctx;
  ctx[ x->name() ] = 4;
  r->fwd_eval( ctx );

  std::cout << "r := ";
  r->print( std::cout );
  std::cout << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;
}

void multivar()
{
  Node::ptr_t x = Variable::make( "x" );
  Node::ptr_t y = Variable::make( "y" );

  Node::ptr_t x2 = Mul::make( x, x );
  Node::ptr_t xy = Mul::make( x, y );

  Node::ptr_t r  = Plus::make( x2, xy );

  Node::context_t ctx;
  ctx[ x->name() ] = 2;
  ctx[ y->name() ] = 3;
  r->fwd_eval( ctx );

  std::cout << "r := " << r << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "          x = " << y->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;
  std::cout << "and   df/dy = " << y->getderiv() << std::endl;

  ctx[ x->name() ] = 7;
  ctx[ y->name() ] = -1;
  r->invalidate();
  r->fwd_eval( ctx );

  std::cout << "r := " << r << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "          y = " << y->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;
  std::cout << "and   df/dy = " << y->getderiv() << std::endl;
}

void polynomial()
{
  Node::ptr_t x   = Variable::make( "x" );
  Node::ptr_t r  = WeightedPoly::make( 1, x, 3 );

  
  Node::context_t ctx;
  ctx[ x->name() ] = 2;
  r->fwd_eval( ctx );

  std::cout << "r := " << r << std::endl;
  std::cout << "given     x = " << x->get() << std::endl;
  std::cout << "find      r = " << r->get() << std::endl;

  r->bkw_deriv_eval( 1.0 );
  std::cout << "and   df/dx = " << x->getderiv() << std::endl;

}

int main( int , char ** )
{
  std::cout << "------------\n";
  super_simple();
#if 0

  std::cout << "------------\n";
  xpx();
  std::cout << "------------\n";
  spoly();
  std::cout << "------------\n";
  divsum();
  std::cout << "------------\n";
  multivar();
  std::cout << "------------\n";
  polynomial();
#endif
  
}
