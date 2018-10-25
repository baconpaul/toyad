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

void print_node_and_derivs( Node::ptr_t &r )
{
  Node::childset_t vars = r->find_children_of_type< Variable >();
  std::cout << "----------------------------------" << std::endl;
  std::cout << "r := " << r << std::endl;
  std::cout << "given:\n";
  for( auto it = vars.begin(); it != vars.end(); ++it )
    {
      std::cout << "     " << *it << " = " << (*it)->getvalue() << std::endl;
    }
  std::cout << "find:\n";
  std::cout << "     r = " << r->getvalue() << std::endl;

  for( auto it = vars.begin(); it != vars.end(); ++it )
    {
      std::cout << " dr/d" << *it << " = " << (*it)->getderiv() << std::endl;
    }
  
}


void super_simple()
{
  auto x   = Variable::make( "x" );
  auto r   = x * 3;
  
  Node::context_t ctx;
  ctx[ x->name() ] = 4;
  
  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

void xpx()
{
  auto x   = Variable::make( "x" );
  auto r   = x + x;
  
  Node::context_t ctx;
  ctx[ x->name() ] = 3;

  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

void spoly()
{
  auto x   = Variable::make( "x" );
  auto r   = x * x - x;
  
  Node::context_t ctx;
  ctx[ x->name() ] = 3;

  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

void divsum()
{
  auto x   = Variable::make( "x" );
  auto r   = ( x + 2 ) / x;
  
  Node::context_t ctx;
  ctx[ x->name() ] = 4;
  
  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );

}

void multivar()
{
  auto x = Variable::make( "x" );
  auto y = Variable::make( "y" );
  auto r = x * x + x * y;

  Node::context_t ctx;
  ctx[ x->name() ] = 2;
  ctx[ y->name() ] = 3;
  
  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
  
  
  r->invalidate();

  ctx[ x->name() ] = 7;
  ctx[ y->name() ] = -1;
  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

void polynomial()
{
  auto x   = Variable::make( "x" );
  auto r   = 3 * (x^3) + 4 * (x^2) - 7 * x;
 
  Node::context_t ctx;
  ctx[ x->name() ] = 2;
  
  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

void specials()
{
  auto x = Variable::make( "x" );
  auto r = exp( x * 2 );

  Node::context_t ctx;
  ctx[ x->name() ] = 3;

  r->fwd_eval( ctx );
  r->bkw_deriv_eval( 1.0 );

  print_node_and_derivs( r );
}

int main( int , char ** )
{
  super_simple();
  xpx();
  spoly();
  divsum();
  multivar();
  polynomial();
  specials();
}
