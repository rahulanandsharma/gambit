//#
//# FILE: gsmoper.cc -- implementations for GSM operator functions
//#                     companion to GSM
//#
//# $Id$
//#

#include <stdlib.h>
#include <ctype.h>

#include "gsm.h"
#include "portion.h"
#include "gsmfunc.h"

#include "rational.h"
#include "gblock.h"



Portion* GSM_Assign_Undefined( Portion** param )
{
  assert( param[0] == 0 );
  if( param[1]->Type() & (porINPUT|porOUTPUT) )
  {
    if( param[1]->Original() == param[1] )
    {
      param[0] = param[1];
      param[1] = param[1]->RefCopy();      
    }
    else
      return 
	new ErrorPortion( "Cannot assign from an INPUT or OUTPUT variable" );
  }
  else
    param[0] = param[1]->ValCopy();
  return param[0]->RefCopy();
}




Portion* GSM_Assign( Portion** param )
{
  Portion* result = 0;

  assert( param[0] != 0 );
  assert( param[1] != 0 );

  if( param[ 0 ]->Type() == param[ 1 ]->Type() )
  {    
    if( param[ 0 ]->Type() != porLIST )
    {
      if( !(param[0]->Type() & (porINPUT|porOUTPUT)) )
      {
	param[ 0 ]->AssignFrom( param[ 1 ] );
	result = param[ 0 ]->RefCopy();
      }
      else
	result = 
	  new ErrorPortion( "Cannot assign from an INPUT or OUTPUT variable" );
    }
    else if((((ListPortion*) param[ 0 ] )->DataType() ==
	     ((ListPortion*) param[ 1 ] )->DataType() ) ||
	    ((ListPortion*) param[ 0 ] )->DataType() == porUNDEFINED )
    {
      if( !( ((ListPortion*)param[0])->DataType() & (porINPUT|porOUTPUT) ) )
      {
	param[ 0 ]->AssignFrom( param[ 1 ] );
	result = param[ 0 ]->RefCopy();
      }
      else
	result = 
	  new ErrorPortion( "Cannot assign from an INPUT or OUTPUT variable" );
    }
    else
    {
      result = new ErrorPortion( "Attempted to change the type of a list" );
    }
  }
  else
  {
    result = new ErrorPortion( "Attempted to change the type of a variable" );
  }

  return result;
}



//-------------------------------------------------------------------
//                      mathematical operators
//------------------------------------------------------------------


//---------------------- GSM_Add ---------------------------

Portion* GSM_Add_double( Portion** param )
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() +
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Add_int( Portion** param )
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() +
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Add_gRational( Portion** param )
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() +
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Add_MixedFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 ||
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<double>*) ( (MixedPortion*) result )->Value() ) +=
    ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 1 ] )->Value() );
  return result;
}

Portion* GSM_Add_MixedRational( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 ||
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<gRational>*) ( (MixedPortion*) result )->Value() ) +=
    ( * (MixedProfile<gRational>*) ( (MixedPortion*) param[ 1 ] )->Value() );
  return result;
}


Portion* GSM_Add_BehavFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 ||
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );

  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<double>*) ( (BehavPortion*) result )->Value() ) +=
    ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 1 ] )->Value() );
  return result;
}

Portion* GSM_Add_BehavRational( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 ||
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<gRational>*) ( (BehavPortion*) result )->Value() ) +=
    ( * (BehavProfile<gRational>*) ( (BehavPortion*) param[ 1 ] )->Value() );
  return result;
}


//--------------------- Concat ---------------------------

Portion* GSM_Concat_gString( Portion** param )
{
  Portion* result = 0;
  result = new TextValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() +
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Concat_List( Portion** param )
{
  Portion* result = 0;
  int i;
  int append_result;

  // gBlock<Portion*>& p_value = ( (ListPortion*) param[ 1 ] )->Value();
  gList<Portion*>& p_value = ( (ListPortion*) param[ 1 ] )->Value();
  result = new ListValPortion( ( (ListPortion*) param[ 0 ] )->Value() );
  for( i = 1; i <= p_value.Length(); i++ )
  {
    append_result = ( (ListPortion*) result )->Append( p_value[ i ]->ValCopy() );
    if( append_result == 0 )
    {
      delete result;
      result = new ErrorPortion
	( "Attempted concatenating lists of different types" );
      break;
    }
  }
  return result;
}



//---------------------- GSM_Subtract ------------------------

Portion* GSM_Subtract_double( Portion** param )
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() -
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Subtract_int( Portion** param )
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() -
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Subtract_gRational( Portion** param )
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() -
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}


Portion* GSM_Subtract_MixedFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 ||
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<double>*) ( (MixedPortion*) result )->Value() ) -=
    ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 1 ] )->Value() );
  return result;
}

Portion* GSM_Subtract_MixedRational( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 ||
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<gRational>*) ( (MixedPortion*) result )->Value() ) -=
    ( * (MixedProfile<gRational>*) ( (MixedPortion*) param[ 1 ] )->Value() );
  return result;
}


Portion* GSM_Subtract_BehavFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 ||
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );

  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<double>*) ( (BehavPortion*) result )->Value() ) -=
    ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 1 ] )->Value() );
  return result;
}

Portion* GSM_Subtract_BehavRational( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 ||
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );

  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<gRational>*) ( (BehavPortion*) result )->Value() ) -=
    ( * (BehavProfile<gRational>*) ( (BehavPortion*) param[ 1 ] )->Value() );
  return result;
}



//------------------------- GSM_Multiply ---------------------------

Portion* GSM_Multiply_double( Portion** param )
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() *
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Multiply_int( Portion** param )
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() *
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_Multiply_gRational( Portion** param )
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() *
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}


Portion* GSM_Multiply_MixedFloat1( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = param[ 1 ]->ValCopy();
  ( * (MixedProfile<double>*) ( (MixedPortion*) result )->Value() ) *=
    ( (FloatPortion*) param[ 0 ] )->Value();
  return result;
}

Portion* GSM_Multiply_MixedFloat2( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<double>*) ( (MixedPortion*) result )->Value() ) *=
    ( (FloatPortion*) param[ 1 ] )->Value();
  return result;
}


Portion* GSM_Multiply_MixedRational1( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = param[ 1 ]->ValCopy();
  ( * (MixedProfile<gRational>*) ( (MixedPortion*) result )->Value() ) *=
    ( (RationalPortion*) param[ 0 ] )->Value();
  return result;
}

Portion* GSM_Multiply_MixedRational2( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = param[ 0 ]->ValCopy();
  ( * (MixedProfile<gRational>*) ( (MixedPortion*) result )->Value() ) *=
    ( (RationalPortion*) param[ 1 ] )->Value();
  return result;
}


Portion* GSM_Multiply_BehavFloat1( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = param[ 1 ]->ValCopy();
  ( * (BehavProfile<double>*) ( (BehavPortion*) result )->Value() ) *=
    ( (FloatPortion*) param[ 0 ] )->Value();
  return result;
}

Portion* GSM_Multiply_BehavFloat2( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<double>*) ( (BehavPortion*) result )->Value() ) *=
    ( (FloatPortion*) param[ 1 ] )->Value();
  return result;
}


Portion* GSM_Multiply_BehavRational1( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = param[ 1 ]->ValCopy();
  ( * (BehavProfile<gRational>*) ( (BehavPortion*) result )->Value() ) *=
    ( (RationalPortion*) param[ 0 ] )->Value();
  return result;
}

Portion* GSM_Multiply_BehavRational2( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = param[ 0 ]->ValCopy();
  ( * (BehavProfile<gRational>*) ( (BehavPortion*) result )->Value() ) *=
    ( (RationalPortion*) param[ 1 ] )->Value();
  return result;
}



//---------------------------- GSM_Divide -------------------------------

Portion* GSM_Divide_double( Portion** param )
{
  Portion* result = 0;
  if( ( (FloatPortion*) param[ 1 ] )->Value() != 0 )
  {
    result = new FloatValPortion
      (
       ( (FloatPortion*) param[ 0 ] )->Value() /
       ( (FloatPortion*) param[ 1 ] )->Value()
       );
  }
  else
  {
    result = new ErrorPortion( "Division by zero" );
  }
  return result;
}


Portion* GSM_Divide_int( Portion** param )
{
  Portion* result = 0;
  if( ( (IntPortion*) param[ 1 ] )->Value() != 0 )
  {
    result = new RationalValPortion( ( (IntPortion*) param[ 0 ] )->Value() );
    ( (RationalPortion*) result )->Value() /= 
      ( (IntPortion*) param[ 1 ] )->Value();
  }
  else
  {
    result = new ErrorPortion( "Division by zero" );
  }
  return result;
}



Portion* GSM_Divide_gRational( Portion** param )
{
  Portion* result = 0;
  if( ( (RationalPortion*) param[ 1 ] )->Value() != (gRational)0 )
  {
    result = new RationalValPortion
      (
       ( (RationalPortion*) param[ 0 ] )->Value() /
       ( (RationalPortion*) param[ 1 ] )->Value()
       );
  }
  else
  {
    result = new ErrorPortion( "Division by zero" );
  }
  return result;
}



//-------------------------- GSM_IntegerDivide -------------------------------

Portion* GSM_IntegerDivide_int( Portion** param )
{
  Portion* result = 0;
  if( ( (IntPortion*) param[ 1 ] )->Value() != 0 )
  {
    result = new IntValPortion
      (
       ( (IntPortion*) param[ 0 ] )->Value() /
       ( (IntPortion*) param[ 1 ] )->Value()
       );
  }
  else
  {
    result = new ErrorPortion( "Division by zero" );
  }
  return result;
}




//---------------------- GSM_Exp, GSM_Log --------------------

Portion *GSM_Exp(Portion **param)
{
  return new FloatValPortion
    (exp(((FloatPortion *) param[0])->Value()));
}

Portion *GSM_Log(Portion **param)
{
  return new FloatValPortion
    (log(((FloatPortion *) param[0])->Value()));
}


//------------------------ GSM_Power ------------------------

Portion* GSM_Power_Int_Int(Portion** param)
{
  long base = ((IntPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new IntValPortion( (long) pow( (double) base, (double) exponent ) );
}

Portion* GSM_Power_Float_Int(Portion** param)
{
  double base = ((FloatPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new FloatValPortion( pow( base, (double) exponent ) );
}

Portion* GSM_Power_Rational_Int(Portion** param)
{
  gRational base = ((RationalPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new RationalValPortion( pow( base, exponent ) );
}


Portion* GSM_Power_Int_Float(Portion** param)
{
  long base = ((IntPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion( (double) pow( (double) base, exponent ) );
}

Portion* GSM_Power_Float_Float(Portion** param)
{
  double base = ((FloatPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion( (double) pow( base, exponent ) );
}

Portion* GSM_Power_Rational_Float(Portion** param)
{
  gRational base = ((RationalPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion( (double) pow( base, ((long) exponent) ) );
}




//------------------------ GSM_Negate ------------------------

Portion* GSM_Negate_double( Portion** param )
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     -( (FloatPortion*) param[ 0 ] )->Value()
     );
  return result;
}

Portion* GSM_Negate_int( Portion** param )
{
  Portion* result = 0;
  result = new IntValPortion
    (
     -( (IntPortion*) param[ 0 ] )->Value()
     );
  return result;
}

Portion* GSM_Negate_gRational( Portion** param )
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     -( (RationalPortion*) param[ 0 ] )->Value()
     );
  return result;
}



//--------------------------------------------------------------------
//                    integer math operators
//--------------------------------------------------------------------

//---------------------- GSM_Modulus ---------------------------

Portion* GSM_Modulus_int( Portion** param )
{
  Portion* result = 0;
  if( ( (IntPortion*) param[ 1 ] )->Value() != 0 )
  {
    result = new IntValPortion
      (
       ( (IntPortion*) param[ 0 ] )->Value() %
       ( (IntPortion*) param[ 1 ] )->Value()
       );
  }
  else
  {
    result = new ErrorPortion( "Division by zero" );
  }
  return result;
}



//-------------------------------------------------------------------
//                      relational operators
//------------------------------------------------------------------

//---------------------- GSM_EqualTo --------------------------

Portion* GSM_EqualTo_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() ==
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() ==
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() ==
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() ==
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_bool( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (BoolPortion*) param[ 0 ] )->Value() ==
     ( (BoolPortion*) param[ 1 ] )->Value()
     );
  return result;
}






Portion* GSM_EqualTo_EfPlayer( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (EfPlayerPortion*) param[ 0 ] )->Value() ==
     ( (EfPlayerPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_Node( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (NodePortion*) param[ 0 ] )->Value() ==
     ( (NodePortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_Infoset( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (InfosetPortion*) param[ 0 ] )->Value() ==
     ( (InfosetPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_Outcome( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (OutcomePortion*) param[ 0 ] )->Value() ==
     ( (OutcomePortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_EqualTo_Action( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (ActionPortion*) param[ 0 ] )->Value() ==
     ( (ActionPortion*) param[ 1 ] )->Value()
     );
  return result;
}



Portion* GSM_EqualTo_MixedFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 || 
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = new BoolValPortion
    (
     ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 0 ] )->Value() ) ==
     ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 1 ] )->Value() )
     );
  return result;
}

Portion* GSM_EqualTo_MixedRational( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 || 
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = new BoolValPortion
    (
     (* (MixedProfile<gRational>*) ( (MixedPortion*) param[ 0 ] )->Value() ) ==
     (* (MixedProfile<gRational>*) ( (MixedPortion*) param[ 1 ] )->Value() )
     );
  return result;
}


Portion* GSM_EqualTo_BehavFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 || 
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = new BoolValPortion
    (
     ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 0 ] )->Value() ) ==
     ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 1 ] )->Value() )
     );
  return result;
}

Portion* GSM_EqualTo_BehavRational( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 || 
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = new BoolValPortion
    (
     (* (BehavProfile<gRational>*) ( (BehavPortion*) param[ 0 ] )->Value() ) ==
     (* (BehavProfile<gRational>*) ( (BehavPortion*) param[ 1 ] )->Value() )
     );
  return result;
}



//----------------------- GSM_NotEqualTo --------------------------

Portion* GSM_NotEqualTo_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() !=
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() !=
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() !=
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() !=
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_bool( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (BoolPortion*) param[ 0 ] )->Value() !=
     ( (BoolPortion*) param[ 1 ] )->Value()
     );
  return result;
}






Portion* GSM_NotEqualTo_EfPlayer( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (EfPlayerPortion*) param[ 0 ] )->Value() !=
     ( (EfPlayerPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_Node( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (NodePortion*) param[ 0 ] )->Value() !=
     ( (NodePortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_Infoset( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (InfosetPortion*) param[ 0 ] )->Value() !=
     ( (InfosetPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_Outcome( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (OutcomePortion*) param[ 0 ] )->Value() !=
     ( (OutcomePortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_NotEqualTo_Action( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (ActionPortion*) param[ 0 ] )->Value() !=
     ( (ActionPortion*) param[ 1 ] )->Value()
     );
  return result;
}



Portion* GSM_NotEqualTo_MixedFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 || 
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = new BoolValPortion
    (
     ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 0 ] )->Value() ) !=
     ( * (MixedProfile<double>*) ( (MixedPortion*) param[ 1 ] )->Value() )
     );
  return result;
}

Portion* GSM_NotEqualTo_MixedRational( Portion** param )
{
  Portion* result = 0;
  if( ( (MixedPortion*) param[ 0 ] )->Value() == 0 || 
     ( (MixedPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Mixed value" );
  
  result = new BoolValPortion
    (
     (* (MixedProfile<gRational>*) ( (MixedPortion*) param[ 0 ] )->Value() ) !=
     (* (MixedProfile<gRational>*) ( (MixedPortion*) param[ 1 ] )->Value() )
     );
  return result;
}


Portion* GSM_NotEqualTo_BehavFloat( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 || 
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = new BoolValPortion
    (
     ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 0 ] )->Value() ) !=
     ( * (BehavProfile<double>*) ( (BehavPortion*) param[ 1 ] )->Value() )
     );
  return result;
}

Portion* GSM_NotEqualTo_BehavRational( Portion** param )
{
  Portion* result = 0;
  if( ( (BehavPortion*) param[ 0 ] )->Value() == 0 || 
     ( (BehavPortion*) param[ 1 ] )->Value() == 0 )
    return new ErrorPortion( "Cannot operate on a null Behav value" );
  
  result = new BoolValPortion
    (
     (* (BehavProfile<gRational>*) ( (BehavPortion*) param[ 0 ] )->Value() ) !=
     (* (BehavProfile<gRational>*) ( (BehavPortion*) param[ 1 ] )->Value() )
     );
  return result;
}




//-------------------------- GSM_GreaterThan -------------------------

Portion* GSM_GreaterThan_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() >
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThan_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() >
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThan_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() >
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThan_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() >
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}


//----------------------- GSM_LessThan -----------------------

Portion* GSM_LessThan_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() <
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThan_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() <
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThan_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() <
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThan_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() <
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}


//--------------------- GSM_GreaterThanOrEqualTo --------------------

Portion* GSM_GreaterThanOrEqualTo_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() >=
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() >=
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() >=
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() >=
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}


//--------------------- GSM_LessThanOrEqualTo ---------------------

Portion* GSM_LessThanOrEqualTo_double( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (FloatPortion*) param[ 0 ] )->Value() <=
     ( (FloatPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThanOrEqualTo_int( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (IntPortion*) param[ 0 ] )->Value() <=
     ( (IntPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThanOrEqualTo_gRational( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (RationalPortion*) param[ 0 ] )->Value() <=
     ( (RationalPortion*) param[ 1 ] )->Value()
     );
  return result;
}

Portion* GSM_LessThanOrEqualTo_gString( Portion** param )
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ( (TextPortion*) param[ 0 ] )->Value() <=
     ( (TextPortion*) param[ 1 ] )->Value()
     );
  return result;
}




//----------------------------------------------------------------
//                       logical operators
//----------------------------------------------------------------



Portion* GSM_AND( Portion** param )
{
  Portion* result = 0;

  assert( param[ 0 ]->Type() == porBOOL );
  
  result = new BoolValPortion
    (
     ( (BoolPortion*) param[ 0 ] )->Value() &&
     ( (BoolPortion*) param[ 1 ] )->Value()
     );
  return result;
}



Portion* GSM_OR( Portion** param )
{
  Portion* result = 0;

  assert( param[ 0 ]->Type() == porBOOL );
  
  result = new BoolValPortion
    (
     ( (BoolPortion*) param[ 0 ] )->Value() ||
     ( (BoolPortion*) param[ 1 ] )->Value()
     );

  return result;
}



Portion* GSM_NOT( Portion** param )
{
  Portion* result = 0;

  assert( param[ 0 ]->Type() == porBOOL );
  
  result = new BoolValPortion( !( (BoolPortion*) param[ 0 ] )->Value() );

  return result;
}


Portion *GSM_Paren(Portion **param)
{
  return param[0]->ValCopy();
}

//-----------------------------------------------------------------
//    NewStream function - possibly belong somewhere else
//-----------------------------------------------------------------


Portion* GSM_NewOutputStream( Portion** param )
{
  Portion* result = 0;
  gOutput* g;
  
  assert( param[ 0 ]->Type() == porTEXT );

  g = new gFileOutput( ( (TextPortion*) param[ 0 ] )->Value() );
  result = new OutputValPortion( *g );
  
  return result;
}


Portion* GSM_NewInputStream( Portion** param )
{
  Portion* result = 0;
  gInput* g;
  
  assert( param[ 0 ]->Type() == porTEXT );

  g = new gFileInput( ( (TextPortion*) param[ 0 ] )->Value() );

  if( g->IsValid() )
    result = new InputValPortion( *g );
  else
    result = new ErrorPortion( (gString) "Error opening file \"" + 
			      ( (TextPortion*) param[ 0 ] )->Value() + "\"" );
  
  return result;
}




//-----------------------------------------------------------------
//    Write and SetFormat function - possibly belong somewhere else
//-----------------------------------------------------------------

long _WriteWidth = 0;
long _WritePrecis = 6;
bool _WriteExpmode = false;
bool _WriteQuoted = true;
bool _WriteListBraces = true;
bool _WriteListCommas = true;
long _WriteListLF = 0;
long _WriteListIndent = 2;



void GSM_SetWriteOptions( void )
{
  Portion::_SetWriteWidth( _WriteWidth );
  Portion::_SetWritePrecis( _WritePrecis );
  Portion::_SetWriteExpmode( _WriteExpmode );
  Portion::_SetWriteQuoted( _WriteQuoted );
  Portion::_SetWriteListBraces( _WriteListBraces );
  Portion::_SetWriteListCommas( _WriteListCommas );
  Portion::_SetWriteListLF( _WriteListLF );
  Portion::_SetWriteListIndent( _WriteListIndent );
}


Portion* GSM_SetFormat( Portion** param )
{
  _WriteWidth = ( (IntPortion*) param[ 0 ] )->Value();
  _WritePrecis = ( (IntPortion*) param[ 1 ] )->Value();
  _WriteExpmode = ( (BoolPortion*) param[ 2 ] )->Value();
  _WriteQuoted = ( (BoolPortion*) param[ 3 ] )->Value();
  _WriteListBraces = ( (BoolPortion*) param[ 4 ] )->Value();
  _WriteListCommas = ( (BoolPortion*) param[ 5 ] )->Value();
  _WriteListLF = ( (IntPortion*) param[ 6 ] )->Value();
  _WriteListIndent = ( (IntPortion*) param[ 7 ] )->Value();

  GSM_SetWriteOptions();

  return new BoolValPortion( true );
}





Portion* GSM_Write_numerical( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();
  s << param[ 1 ];
  return param[0]->RefCopy();
}


Portion* GSM_Write_gString( Portion** param )
{
  int i = 0;

  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();

  gString text = ( (TextPortion*) param[ 1 ] )->Value();

  for( i = 0; i < text.length(); i++ )
  {
    if( text[ i ] == '\\' && text[ i + 1 ] == 'n' )
    {
      text.remove( i );
      text[ i ] = '\n';
    }
  }

  s << text;
  return param[0]->RefCopy();
}


Portion* GSM_Write_Mixed( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();

  BaseMixedProfile* mixed = ( (MixedPortion*) param[ 1 ] )->Value();
  switch( mixed->Type() )
  {
  case DOUBLE:
    s << * (MixedProfile<double>*) mixed; 
    break;
  case RATIONAL:
    s << * (MixedProfile<gRational>*) mixed; 
    break;
  default:
    assert( 0 );
  }
  return param[0]->RefCopy();
}


Portion* GSM_Write_Behav( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();

  BaseBehavProfile* behav = ( (BehavPortion*) param[ 1 ] )->Value();
  switch( behav->Type() )
  {
  case DOUBLE:
    s << * (BehavProfile<double>*) behav; 
    break;
  case RATIONAL:
    s << * (BehavProfile<gRational>*) behav; 
    break;
  default:
    assert( 0 );
  }
  return param[0]->RefCopy();
}


Portion* GSM_Write_Nfg( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();

  BaseNfg* nfg = ( (NfgPortion*) param[ 1 ] )->Value();
  // nfg->DisplayNfg( s );

  s << "Temporary hack; still waiting for DisplayNfg() to finish\n";
  nfg->WriteNfgFile( s );

  return param[0]->RefCopy();
}


Portion* GSM_Write_Efg( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();

  BaseEfg* efg = ( (EfgPortion*) param[ 1 ] )->Value();
  // efg->DisplayEfg( s );

  s << "Temporary hack; still waiting for DisplayEfg() to finish\n";
  efg->WriteEfgFile( s );

  return param[0]->RefCopy();
}




Portion* GSM_Write_list( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();
  s << param[ 1 ];
  return param[0]->RefCopy();
}


Portion* GSM_Write_list_Text( Portion** param );

Portion* GSM_Write_list_Text( Portion** param )
{
  gOutput& s = ( (OutputPortion*) param[ 0 ] )->Value();
  ListPortion* list = ( (ListPortion*) param[ 1 ] );
  int i;
  Portion* p;
  int length = list->Length();
  Portion** subparam;

  s << "{";
  if( length >= 1 )
  {
    for( i = 1; i <= length; i++ )
    {
      p = (*list)[i];
      if( i > 1 )
	s << ",";
      if( p->IsValid() )
      {
	assert( p->Type() == porTEXT || p->Type() == porLIST );
	if( p->Type() == porTEXT )
	{
	  if( _WriteQuoted )
	    s << " \"" << ((TextPortion*)p)->Value() << "\"";
	  else
	    s << " " << ((TextPortion*)p)->Value();
	}
	else
	{
	  s << ' ';
	  subparam = new Portion*[2];
	  subparam[1] = p;
	  subparam[0] = param[0];
	  GSM_Write_list_Text( subparam );
	  delete[] subparam;
	}
      }
      else
	s << " (undefined)";
    }
  }
  else
  {
    s << " empty";
  }
  s << " }";

  return param[0]->RefCopy();
}




//------------------------------ Read --------------------------//





Portion* GSM_Read_Bool( Portion** param )
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  bool value;
  bool error = false;
  char c = ' ';

  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  while( !input.eof() && isspace(c) )
    input.get(c);
  if( c == 'T' )
  {
    if( !input.eof() ) input.get(c); if( c != 'r' ) error = true;
    if( !input.eof() ) input.get(c); if( c != 'u' ) error = true;
    if( !input.eof() ) input.get(c); if( c != 'e' ) error = true;
    value = true;
  }
  else if( c == 'F' )
  {
    if( !input.eof() ) input.get(c); if( c != 'a' ) error = true;
    if( !input.eof() ) input.get(c); if( c != 'l' ) error = true;
    if( !input.eof() ) input.get(c); if( c != 's' ) error = true;
    if( !input.eof() ) input.get(c); if( c != 'e' ) error = true;
    value = false;
  }
  else
    error = true;

  if( error )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "No boolean data found" );
  }

  ((BoolPortion*) param[1])->Value() = value;
  return param[0]->RefCopy();
}


Portion* GSM_Read_Integer( Portion** param )
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  int value;
  char c;

  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  input >> value;
  if( !input.IsValid() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "File read error" );
  }

  input.get(c);
  while( !input.eof() && isspace(c) ) 
    input.get(c); 
  if( c == '/' )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Type mismatch: expected INTEGER, got RATIONAL" );
  }
  else if( c == '.' )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Type mismatch: expected INTEGER, got FLOAT" );
  }
  else
    input.unget(c);

  ((IntPortion*) param[1])->Value() = value;

  return param[0]->RefCopy();
}

Portion* GSM_Read_Float( Portion** param )
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  double value;
  char c;

  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  input >> value;
  if( !input.IsValid() ) 
  {
    input.setpos( old_pos );
    return new ErrorPortion( "File read error" );
  }

  input.get(c);
  while( !input.eof() && isspace(c) ) 
    input.get(c); 
  if( c == '/' )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Type mismatch: expected FLOAT, got RATIONAL" );
  }
  else
    input.unget(c);

  ((FloatPortion*) param[1])->Value() = value;

  return param[0]->RefCopy();
}

Portion* GSM_Read_Rational( Portion** param )
{
  int numerator = 0;
  int denominator = 0;
  char c = ' ';
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  input >> numerator;
  if( !input.IsValid() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "File read error" );
  }

  input.get(c);
  while( !input.eof() && isspace(c) ) 
    input.get(c); 
  if( !input.eof() && c == '.' )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Expected RATIONAL, got FLOAT" );
  }
  if( input.eof() || c != '/' )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Expected RATIONAL, got INTEGER" );
  }

  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  input >> denominator;
  if( !input.IsValid() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "File read error" );
  }

  if( denominator == 0 )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "Division by zero" );
  }

  ((RationalPortion*) param[1])->Value() = numerator;
  ((RationalPortion*) param[1])->Value() /= denominator;
 
  return param[0]->RefCopy();
}

Portion* GSM_Read_Text( Portion** param )
{
  char c = ' ';
  gString s;
  gString t;
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  while( !input.eof() && isspace(c) )
    input.get( c );
  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  if( !input.eof() && c == '\"' )
    input.get(c); 
  else
  {
    input.unget(c);
    input.setpos( old_pos );
    return new ErrorPortion( "File read error: missing starting \"" );
  }

  while( !input.eof() && c != '\"' )
  { t+=c; input.get(c); }
  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );  
  }
  
  ((TextPortion*) param[1])->Value() = t;
 
  return param[0]->RefCopy();
}



Portion* GSM_Read_List( Portion** param, PortionType type,
		       Portion* (*func) ( Portion** ), bool ListFormat )
{
  Portion* p;
  Portion** sub_param;
  ListPortion* list;
  int i;
  char c = ' ';  
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();


  while( !input.eof() && isspace(c) )
    input.get( c );
  if( input.eof() )
  {
    input.setpos( old_pos );
    return new ErrorPortion( "End of file reached" );
  }
  if( !ListFormat )
  {
    if( c == '{' )
      ListFormat = true;
    else
      input.unget(c);
  }
  else
  {
    if( c != '{' )
    {
      input.setpos( old_pos );
      return new ErrorPortion( "\'{\' expected" );
    }
  }


  sub_param = new Portion*[2];
  list = ((ListPortion*) param[1]);


  for( i=1; i <= list->Length(); i++ )
  {
    assert( (*list)[i]->Type()==type || (*list)[i]->Type()==porLIST );
    sub_param[0] = param[0];
    sub_param[1] = (*list)[i];

    if( i > 1 ) 
    {
      c = ' ';
      while( !input.eof() && isspace(c) )
	input.get( c );
      if( c == ',' )
      {
	if( !ListFormat )
	  input.unget(c);
      }
      else
	input.unget(c);      
    }

    if( (*list)[i]->Type() == type )
      p = (*func)( sub_param );
    else
      p = GSM_Read_List( sub_param, type, func, ListFormat );

    if( p->Type() == porERROR )
    {
      delete[] sub_param;
      input.setpos(old_pos);
      return p;
    }
    else
      delete p;
  }
  delete[] sub_param;

  if( ListFormat )
  {
    c = ' ';
    while( !input.eof() && isspace(c) )
      input.get(c);
    if( c != '}' )
    {
      input.setpos(old_pos);
      return new ErrorPortion( "Mismatched braces" );
    }
    if( input.eof() )
    {
      input.setpos( old_pos );
      return new ErrorPortion( "End of file reached" );
    }
  }

  return param[0]->RefCopy();
}



Portion* GSM_Read_List_Bool( Portion** param )
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List( param, porBOOL, GSM_Read_Bool, false );
  if( p->Type() == porERROR )
    param[1]->AssignFrom( temp );
  delete temp;
  return p;
}

Portion* GSM_Read_List_Integer( Portion** param )
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List( param, porINTEGER, GSM_Read_Integer, false );
  if( p->Type() == porERROR )
    param[1]->AssignFrom( temp );
  delete temp;
  return p;
}

Portion* GSM_Read_List_Float( Portion** param )
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List( param, porFLOAT, GSM_Read_Float, false );
  if( p->Type() == porERROR )
    param[1]->AssignFrom( temp );
  delete temp;
  return p;
}

Portion* GSM_Read_List_Rational( Portion** param )
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List( param, porRATIONAL, GSM_Read_Rational, false );
  if( p->Type() == porERROR )
    param[1]->AssignFrom( temp );
  delete temp;
  return p;
}

Portion* GSM_Read_List_Text( Portion** param )
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List( param, porTEXT, GSM_Read_Text, false );
  if( p->Type() == porERROR )
    param[1]->AssignFrom( temp );
  delete temp;
  return p;
}




extern Portion* GSM_List_MixedFloat( Portion** param );
extern Portion* GSM_Mixed_NfgFloat( Portion **param );

Portion* GSM_Read_MixedFloat( Portion** param )
{
  Portion** sub_param = new Portion*[2];
  Portion* owner = param[1]->Owner();
  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_List_MixedFloat( sub_param );
  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Float( sub_param );
  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Mixed_NfgFloat( sub_param );
  param[1]->AssignFrom( p );
  delete list;
  delete p;
  delete[] sub_param;
  return result;
}


extern Portion* GSM_List_MixedRational( Portion** param );
extern Portion* GSM_Mixed_NfgRational( Portion **param );

Portion* GSM_Read_MixedRational( Portion** param )
{
  Portion** sub_param = new Portion*[2];
  Portion* owner = param[1]->Owner();
  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_List_MixedRational( sub_param );
  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Rational( sub_param );
  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Mixed_NfgRational( sub_param );
  param[1]->AssignFrom( p );
  delete list;
  delete p;
  delete[] sub_param;
  return result;
}



extern Portion* GSM_List_BehavFloat( Portion** param );
extern Portion* GSM_Behav_EfgFloat( Portion **param );

Portion* GSM_Read_BehavFloat( Portion** param )
{
  Portion** sub_param = new Portion*[2];
  Portion* owner = param[1]->Owner();
  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_List_BehavFloat( sub_param );
  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Float( sub_param );
  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Behav_EfgFloat( sub_param );
  param[1]->AssignFrom( p );
  delete list;
  delete p;
  delete[] sub_param;
  return result;
}


extern Portion* GSM_List_BehavRational( Portion** param );
extern Portion* GSM_Behav_EfgRational( Portion **param );

Portion* GSM_Read_BehavRational( Portion** param )
{
  Portion** sub_param = new Portion*[2];
  Portion* owner = param[1]->Owner();
  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_List_BehavRational( sub_param );
  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Rational( sub_param );
  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Behav_EfgRational( sub_param );
  param[1]->AssignFrom( p );
  delete list;
  delete p;
  delete[] sub_param;
  return result;
}




Portion* GSM_Read_Undefined( Portion** param )
{
  /* will go through and try to read the input as different format until
     it succeeds */

  Portion* p;
  Portion** sub_param;
  ListPortion* list;
  int i;
  char c = ' ';  
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  Portion* result = 0;

  assert( param[1] == 0 );


  while( !input.eof() && isspace(c) )
    input.get(c);
  if( input.eof() )
    return new ErrorPortion( "End of file reached" );
  if( c == '{' )
  {
    param[1] = new ListValPortion();

    sub_param = new Portion*[2];
    
    do
    {
      sub_param[0] = param[0];
      sub_param[1] = 0;
      result = GSM_Read_Undefined( sub_param );
      if( result->Type() != porERROR )
	((ListPortion*) param[1])->Append( sub_param[1] );
      else
      {
	delete result;
	result = 0;
      }

      c = ' ';
      while( !input.eof() && isspace(c) )
	input.get(c);
      if( !input.eof() && c != ',' )
	input.unget(c);

    } while( result != 0 && !input.eof() );

    delete[] sub_param;

    c = ' ';
    while( !input.eof() && isspace(c) )
      input.get(c);
    if( input.eof() )
    {
      delete result;
      delete param[1];
      param[1] = 0;
      result = new ErrorPortion( "End of file reached" );
    }
    else if( c != '}' )
    {
      delete result;
      delete param[1];
      param[1] = 0;
      result = new ErrorPortion( "Mismatching braces" );
    }
    else
    {
      assert( result == 0 );
      result = param[0]->RefCopy();
    }

  }
  else // not a list
  {
    input.unget(c);
    param[1] = new BoolValPortion( false );
    result = GSM_Read_Bool( param );

    if( result->Type() == porERROR )
    {
      delete param[1];
      delete result;
      param[1] = new IntValPortion( 0 );
      result = GSM_Read_Integer( param );    
    }
    if( result->Type() == porERROR )
    {
      delete param[1];
      delete result;
      param[1] = new FloatValPortion( 0 );
      result = GSM_Read_Float( param );    
    }
    if( result->Type() == porERROR )
    {
      delete param[1];
      delete result;
      param[1] = new RationalValPortion( 0 );
      result = GSM_Read_Rational( param );    
    }
    if( result->Type() == porERROR )
    {
      delete param[1];
      delete result;
      param[1] = new TextValPortion( (gString) "" );
      result = GSM_Read_Text( param );    
    }
    if( result->Type() == porERROR )
    {
      delete param[1];
      delete result;
      param[1] = 0;
      result = new ErrorPortion( "Cannot determine data type" );
      input.setpos( old_pos );
    }

  }

  return result;
}




void Init_gsmoper( GSM* gsm )
{
  FuncDescObj* FuncObj;


  //---------------------- Assign ------------------------
  FuncObj = new FuncDescObj( (gString) "Assign" );
  FuncObj->SetFuncInfo( GSM_Assign, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Assign, 0, "x", porANYTYPE,
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );
  FuncObj->SetParamInfo( GSM_Assign, 1, "y", porANYTYPE );

  FuncObj->SetFuncInfo( GSM_Assign_Undefined, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Assign_Undefined, 0, "x", porUNDEFINED, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );
  FuncObj->SetParamInfo( GSM_Assign_Undefined, 1, "y", porANYTYPE );
  gsm->AddFunction( FuncObj );




  ParamInfoType xy_Int[] =
  {
    ParamInfoType( "x", porINTEGER ),
    ParamInfoType( "y", porINTEGER )
  };

  ParamInfoType xy_Float[] =
  {
    ParamInfoType( "x", porFLOAT ),
    ParamInfoType( "y", porFLOAT )
  };

  ParamInfoType xy_Rational[] =
  {
    ParamInfoType( "x", porRATIONAL ),
    ParamInfoType( "y", porRATIONAL )
  };

  ParamInfoType xy_Text[] =
  {
    ParamInfoType( "x", porTEXT ),
    ParamInfoType( "y", porTEXT )
  };

  ParamInfoType xy_Bool[] =
  {
    ParamInfoType( "x", porBOOL ),
    ParamInfoType( "y", porBOOL )
  };

  ParamInfoType xy_List[] =
  {
    ParamInfoType( "x", porLIST ),
    ParamInfoType( "y", porLIST )
  };


  ParamInfoType xy_MixedFloat[] =
  {
    ParamInfoType( "x", porMIXED_FLOAT ),
    ParamInfoType( "y", porMIXED_FLOAT )
  };

  ParamInfoType xy_MixedRational[] =
  {
    ParamInfoType( "x", porMIXED_RATIONAL ),
    ParamInfoType( "y", porMIXED_RATIONAL )
  };

  ParamInfoType xy_BehavFloat[] =
  {
    ParamInfoType( "x", porBEHAV_FLOAT ),
    ParamInfoType( "y", porBEHAV_FLOAT )
  };

  ParamInfoType xy_BehavRational[] =
  {
    ParamInfoType( "x", porBEHAV_RATIONAL ),
    ParamInfoType( "y", porBEHAV_RATIONAL )
  };


  ParamInfoType xy_EfPlayer[] =
  {
    ParamInfoType( "x", porPLAYER_EFG ),
    ParamInfoType( "y", porPLAYER_EFG )
  };

  ParamInfoType xy_Node[] =
  {
    ParamInfoType( "x", porNODE ),
    ParamInfoType( "y", porNODE )
  };

  ParamInfoType xy_Infoset[] =
  {
    ParamInfoType( "x", porINFOSET ),
    ParamInfoType( "y", porINFOSET )
  };

  ParamInfoType xy_Outcome[] =
  {
    ParamInfoType( "x", porOUTCOME ),
    ParamInfoType( "y", porOUTCOME )
  };

  ParamInfoType xy_Action[] =
  {
    ParamInfoType( "x", porACTION ),
    ParamInfoType( "y", porACTION )
  };



  ParamInfoType x_Float[] =
  {
    ParamInfoType( "x", porFLOAT )
  };


  ParamInfoType file_Text[] =
  {
    ParamInfoType( "file", porTEXT )
  };


  //-------------------- Plus ----------------------------

  FuncObj = new FuncDescObj( (gString) "Plus" );

  FuncObj->SetFuncInfo( GSM_Add_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_Add_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_Add_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_Add_MixedFloat, 2, xy_MixedFloat );
  FuncObj->SetFuncInfo( GSM_Add_MixedRational, 2, xy_MixedRational );
  FuncObj->SetFuncInfo( GSM_Add_BehavFloat, 2, xy_BehavFloat );
  FuncObj->SetFuncInfo( GSM_Add_BehavRational, 2, xy_BehavRational );
  gsm->AddFunction( FuncObj );

  //-------------------- Concat -------------------------
  
  FuncObj = new FuncDescObj("Concat");

  FuncObj->SetFuncInfo( GSM_Concat_gString, 2, xy_Text );
  FuncObj->SetFuncInfo( GSM_Concat_List, 2, xy_List, NON_LISTABLE );
  gsm->AddFunction(FuncObj);

  //----------------------- Minus ------------------------

  FuncObj = new FuncDescObj( (gString) "Minus" );
  FuncObj->SetFuncInfo( GSM_Subtract_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_Subtract_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_Subtract_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_Subtract_MixedFloat, 2, xy_MixedFloat );
  FuncObj->SetFuncInfo( GSM_Add_MixedRational, 2, xy_MixedRational );
  FuncObj->SetFuncInfo( GSM_Add_BehavFloat, 2, xy_BehavFloat );
  FuncObj->SetFuncInfo( GSM_Add_BehavRational, 2, xy_BehavRational );
  gsm->AddFunction( FuncObj );


  //----------------------- Times ----------------------------

  FuncObj = new FuncDescObj( (gString) "Times" );

  FuncObj->SetFuncInfo( GSM_Multiply_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_Multiply_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_Multiply_gRational, 2, xy_Rational );

  FuncObj->SetFuncInfo( GSM_Multiply_MixedFloat1, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_MixedFloat1, 0, "x", porFLOAT );
  FuncObj->SetParamInfo( GSM_Multiply_MixedFloat1, 1, "y", porMIXED_FLOAT );
  FuncObj->SetFuncInfo( GSM_Multiply_MixedFloat2, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_MixedFloat2, 0, "x", porMIXED_FLOAT );
  FuncObj->SetParamInfo( GSM_Multiply_MixedFloat2, 1, "y", porFLOAT );

  FuncObj->SetFuncInfo( GSM_Multiply_MixedRational1, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_MixedRational1, 0, "x", porRATIONAL );
  FuncObj->SetParamInfo( GSM_Multiply_MixedRational1, 
			1, "y", porMIXED_RATIONAL );
  FuncObj->SetFuncInfo( GSM_Multiply_MixedRational2, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_MixedRational2, 
			0, "x", porMIXED_RATIONAL );
  FuncObj->SetParamInfo( GSM_Multiply_MixedRational2, 1, "y", porRATIONAL );

  FuncObj->SetFuncInfo( GSM_Multiply_BehavFloat1, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_BehavFloat1, 0, "x", porFLOAT );
  FuncObj->SetParamInfo( GSM_Multiply_BehavFloat1, 1, "y", porBEHAV_FLOAT );
  FuncObj->SetFuncInfo( GSM_Multiply_BehavFloat2, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_BehavFloat2, 0, "x", porBEHAV_FLOAT );
  FuncObj->SetParamInfo( GSM_Multiply_BehavFloat2, 1, "y", porFLOAT );

  FuncObj->SetFuncInfo( GSM_Multiply_BehavRational1, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_BehavRational1, 0, "x", porRATIONAL );
  FuncObj->SetParamInfo( GSM_Multiply_BehavRational1, 
			1, "y", porBEHAV_RATIONAL );
  FuncObj->SetFuncInfo( GSM_Multiply_BehavRational2, 2 );
  FuncObj->SetParamInfo( GSM_Multiply_BehavRational2, 
			0, "x", porBEHAV_RATIONAL );
  FuncObj->SetParamInfo( GSM_Multiply_BehavRational2, 1, "y", porRATIONAL );

  gsm->AddFunction( FuncObj );


  //----------------------- Divide -------------------------

  FuncObj = new FuncDescObj( (gString) "Divide" );

  FuncObj->SetFuncInfo( GSM_Divide_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_Divide_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_Divide_gRational, 2, xy_Rational );
  gsm->AddFunction( FuncObj );


  //----------------------- Power -------------------------

  FuncObj = new FuncDescObj( (gString) "Power" );

  FuncObj->SetFuncInfo( GSM_Power_Int_Int, 2 );
  FuncObj->SetParamInfo( GSM_Power_Int_Int, 0, "x", porINTEGER );
  FuncObj->SetParamInfo( GSM_Power_Int_Int, 1, "y", porINTEGER );
  FuncObj->SetFuncInfo( GSM_Power_Float_Int, 2 );
  FuncObj->SetParamInfo( GSM_Power_Float_Int, 0, "x", porFLOAT );
  FuncObj->SetParamInfo( GSM_Power_Float_Int, 1, "y", porINTEGER );
  FuncObj->SetFuncInfo( GSM_Power_Rational_Int, 2 );
  FuncObj->SetParamInfo( GSM_Power_Rational_Int, 0, "x", porRATIONAL );
  FuncObj->SetParamInfo( GSM_Power_Rational_Int, 1, "y", porINTEGER );

  FuncObj->SetFuncInfo( GSM_Power_Int_Float, 2 );
  FuncObj->SetParamInfo( GSM_Power_Int_Float, 0, "x", porINTEGER );
  FuncObj->SetParamInfo( GSM_Power_Int_Float, 1, "y", porFLOAT );
  FuncObj->SetFuncInfo( GSM_Power_Float_Float, 2 );
  FuncObj->SetParamInfo( GSM_Power_Float_Float, 0, "x", porFLOAT );
  FuncObj->SetParamInfo( GSM_Power_Float_Float, 1, "y", porFLOAT );
  FuncObj->SetFuncInfo( GSM_Power_Rational_Float, 2 );
  FuncObj->SetParamInfo( GSM_Power_Rational_Float, 0, "x", porRATIONAL );
  FuncObj->SetParamInfo( GSM_Power_Rational_Float, 1, "y", porFLOAT );

  gsm->AddFunction( FuncObj );


  //----------------------- Exp, Log ---------------------

  FuncObj = new FuncDescObj("Exp");
  FuncObj->SetFuncInfo(GSM_Exp, 1, x_Float);
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("Log");
  FuncObj->SetFuncInfo(GSM_Log, 1, x_Float);
  gsm->AddFunction(FuncObj);


  //----------------------- Negate -----------------------

  FuncObj = new FuncDescObj( (gString) "Negate" );

  FuncObj->SetFuncInfo( GSM_Negate_double, 1, x_Float );

  FuncObj->SetFuncInfo( GSM_Negate_int, 1 );
  FuncObj->SetParamInfo( GSM_Negate_int, 0, "x", porINTEGER );

  FuncObj->SetFuncInfo( GSM_Negate_gRational, 1 );
  FuncObj->SetParamInfo( GSM_Negate_gRational, 0, "x", porRATIONAL );

  gsm->AddFunction( FuncObj );



  //----------------------- IntegerDivide ------------------------

  FuncObj = new FuncDescObj( (gString) "IntegerDivide" );
  FuncObj->SetFuncInfo( GSM_IntegerDivide_int, 2, xy_Int );
  gsm->AddFunction( FuncObj );



  //----------------------- Modulus ------------------------

  FuncObj = new FuncDescObj( (gString) "Modulus" );
  FuncObj->SetFuncInfo( GSM_Modulus_int, 2, xy_Int );
  gsm->AddFunction( FuncObj );





  //-------------------------- Equal ---------------------------

  FuncObj = new FuncDescObj( (gString) "Equal" );

  FuncObj->SetFuncInfo( GSM_EqualTo_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_EqualTo_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_EqualTo_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_EqualTo_gString, 2, xy_Text );
  FuncObj->SetFuncInfo( GSM_EqualTo_bool, 2, xy_Bool );

  FuncObj->SetFuncInfo( GSM_EqualTo_EfPlayer, 2, xy_EfPlayer );
  FuncObj->SetFuncInfo( GSM_EqualTo_Node, 2, xy_Node );
  FuncObj->SetFuncInfo( GSM_EqualTo_Infoset, 2, xy_Infoset );
  FuncObj->SetFuncInfo( GSM_EqualTo_Outcome, 2, xy_Outcome );
  FuncObj->SetFuncInfo( GSM_EqualTo_Action, 2, xy_Action );

  FuncObj->SetFuncInfo( GSM_EqualTo_MixedFloat, 2, xy_MixedFloat );
  FuncObj->SetFuncInfo( GSM_EqualTo_MixedRational, 2, xy_MixedRational );
  FuncObj->SetFuncInfo( GSM_EqualTo_BehavFloat, 2, xy_BehavFloat );
  FuncObj->SetFuncInfo( GSM_EqualTo_BehavRational, 2, xy_BehavRational );

  gsm->AddFunction( FuncObj );


  //-------------------------- NotEqual ---------------------------

  FuncObj = new FuncDescObj( (gString) "NotEqual" ); 

  FuncObj->SetFuncInfo( GSM_NotEqualTo_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_gString, 2, xy_Text );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_bool, 2, xy_Bool );

  FuncObj->SetFuncInfo( GSM_NotEqualTo_EfPlayer, 2, xy_EfPlayer );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_Node, 2, xy_Node );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_Infoset, 2, xy_Infoset );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_Outcome, 2, xy_Outcome );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_Action, 2, xy_Action );

  FuncObj->SetFuncInfo( GSM_NotEqualTo_MixedFloat, 2, xy_MixedFloat );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_MixedRational, 2, xy_MixedRational );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_BehavFloat, 2, xy_BehavFloat );
  FuncObj->SetFuncInfo( GSM_NotEqualTo_BehavRational, 2, xy_BehavRational );

  gsm->AddFunction( FuncObj );


  //-------------------------- Greater ---------------------------

  FuncObj = new FuncDescObj( (gString) "Greater" );

  FuncObj->SetFuncInfo( GSM_GreaterThan_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_GreaterThan_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_GreaterThan_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_GreaterThan_gString, 2, xy_Text );

  gsm->AddFunction( FuncObj );


  //-------------------------- Less ---------------------------

  FuncObj = new FuncDescObj( (gString) "Less" );

  FuncObj->SetFuncInfo( GSM_LessThan_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_LessThan_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_LessThan_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_LessThan_gString, 2, xy_Text );

  gsm->AddFunction( FuncObj );


  //---------------------- GreaterThanOrEqualTo ---------------------------

  FuncObj = new FuncDescObj( (gString) "GreaterEqual" );

  FuncObj->SetFuncInfo( GSM_GreaterThanOrEqualTo_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_GreaterThanOrEqualTo_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_GreaterThanOrEqualTo_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_GreaterThanOrEqualTo_gString, 2, xy_Text );

  gsm->AddFunction( FuncObj );


  //----------------------- LessThanOrEqualTo ---------------------------

  FuncObj = new FuncDescObj( (gString) "LessEqual" );

  FuncObj->SetFuncInfo( GSM_LessThanOrEqualTo_double, 2, xy_Float );
  FuncObj->SetFuncInfo( GSM_LessThanOrEqualTo_int, 2, xy_Int );
  FuncObj->SetFuncInfo( GSM_LessThanOrEqualTo_gRational, 2, xy_Rational );
  FuncObj->SetFuncInfo( GSM_LessThanOrEqualTo_gString, 2, xy_Text );

  gsm->AddFunction( FuncObj );




  //--------------------- And -------------------------------

  FuncObj = new FuncDescObj( (gString) "And" );
  FuncObj->SetFuncInfo( GSM_AND, 2, xy_Bool );
  gsm->AddFunction( FuncObj );


  //--------------------- Or -------------------------------

  FuncObj = new FuncDescObj( (gString) "Or" );
  FuncObj->SetFuncInfo( GSM_OR, 2, xy_Bool );
  gsm->AddFunction( FuncObj );


  //--------------------- Not -------------------------------

  FuncObj = new FuncDescObj( (gString) "Not" );
  FuncObj->SetFuncInfo( GSM_NOT, 1 );
  FuncObj->SetParamInfo( GSM_NOT, 0, "x", porBOOL );
  gsm->AddFunction( FuncObj );






  FuncObj = new FuncDescObj("Paren");
  FuncObj->SetFuncInfo(GSM_Paren, 1);
  FuncObj->SetParamInfo(GSM_Paren, 0, "x", porANYTYPE);
  gsm->AddFunction(FuncObj);

  //-------------------- NewStream -------------------------

  FuncObj = new FuncDescObj( (gString) "Output" );
  FuncObj->SetFuncInfo( GSM_NewOutputStream, 1, file_Text );
  gsm->AddFunction( FuncObj );

  FuncObj = new FuncDescObj( (gString) "Input" );
  FuncObj->SetFuncInfo( GSM_NewInputStream, 1, file_Text );
  gsm->AddFunction( FuncObj );

  //------------------- Formatted Output -------------------

  GSM_SetWriteOptions();

  FuncObj = new FuncDescObj( "Write" );
  FuncObj->SetFuncInfo( GSM_Write_numerical, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_numerical, 0, "output", porOUTPUT, 
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_numerical, 1, "x", 
			porBOOL | porINTEGER | porFLOAT | porRATIONAL );

  FuncObj->SetFuncInfo( GSM_Write_gString, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_gString, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_gString, 1, "x", porTEXT );

  FuncObj->SetFuncInfo( GSM_Write_Mixed, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_Mixed, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_Mixed, 1, "x", porMIXED );

  FuncObj->SetFuncInfo( GSM_Write_Behav, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_Behav, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_Behav, 1, "x", porBEHAV );

  FuncObj->SetFuncInfo( GSM_Write_Nfg, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_Nfg, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_Nfg, 1, "x", porNFG,
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Write_Efg, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_Efg, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_Efg, 1, "x", porEFG,
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Write_list, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_list, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_list, 1, "x", 
			porLIST | porBOOL | porINTEGER | porFLOAT | porTEXT |
			porRATIONAL | porMIXED | porBEHAV );

  /*
  FuncObj->SetFuncInfo( GSM_Write_list_Text, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Write_list_Text, 0, "output", porOUTPUT,
			new OutputRefPortion( gout ) );
  FuncObj->SetParamInfo( GSM_Write_list_Text, 1, "x", 
			porLIST | porTEXT );
			*/

  gsm->AddFunction( FuncObj );


  FuncObj = new FuncDescObj( "SetFormat" );
  FuncObj->SetFuncInfo( GSM_SetFormat, 8 );
  FuncObj->SetParamInfo( GSM_SetFormat, 0, "width", porINTEGER, 
			new IntRefPortion( _WriteWidth ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 1, "precis", porINTEGER,
			new IntRefPortion( _WritePrecis ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 2, "expmode", porBOOL,
			new BoolRefPortion( _WriteExpmode ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 3, "quote", porBOOL,
			new BoolRefPortion( _WriteQuoted ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 4, "listBraces", porBOOL,
			new BoolRefPortion( _WriteListBraces ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 5, "listCommas", porBOOL,
			new BoolRefPortion( _WriteListCommas ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 6, "listLF", porINTEGER,
			new IntRefPortion( _WriteListLF ) );
  FuncObj->SetParamInfo( GSM_SetFormat, 7, "listIndent", porINTEGER,
			new IntRefPortion( _WriteListIndent ) );
  gsm->AddFunction( FuncObj );



  //-------------------- Read --------------------------

  FuncObj = new FuncDescObj( "Read" );
  /*
  FuncObj->SetFuncInfo( GSM_Read, 1 );
  FuncObj->SetParamInfo( GSM_Read, 0, "input", porINPUT );
  */

  FuncObj->SetFuncInfo( GSM_Read_Bool, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Bool, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Bool, 1, "x", porBOOL, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_List_Bool, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_List_Bool, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_List_Bool, 1, "x", porBOOL | porLIST, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE, 1 );


  FuncObj->SetFuncInfo( GSM_Read_Integer, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Integer, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Integer, 1, "x", porINTEGER, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_List_Integer, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_List_Integer, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_List_Integer, 1, "x", porINTEGER | porLIST, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE, 1 );


  FuncObj->SetFuncInfo( GSM_Read_Float, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Float, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Float, 1, "x", porFLOAT,  
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_List_Float, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_List_Float, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_List_Float, 1, "x", porFLOAT | porLIST, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE, 1 );


  FuncObj->SetFuncInfo( GSM_Read_Rational, 2,
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Rational, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Rational, 1, "x", porRATIONAL, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_List_Rational, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_List_Rational, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_List_Rational, 1, "x", porRATIONAL|porLIST, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE, 1 );


  FuncObj->SetFuncInfo( GSM_Read_Text, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Text, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Text, 1, "x", porTEXT, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_List_Text, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_List_Text, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_List_Text, 1, "x", porTEXT | porLIST, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE, 1 );



  FuncObj->SetFuncInfo( GSM_Read_MixedFloat, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_MixedFloat, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_MixedFloat, 1, "x", porMIXED_FLOAT, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_MixedRational, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_MixedRational, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_MixedRational, 1, "x", porMIXED_RATIONAL, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );


  FuncObj->SetFuncInfo( GSM_Read_BehavFloat, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_BehavFloat, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_BehavFloat, 1, "x", porBEHAV_FLOAT, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );

  FuncObj->SetFuncInfo( GSM_Read_BehavRational, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_BehavRational, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_BehavRational, 1, "x", porBEHAV_RATIONAL, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );



  FuncObj->SetFuncInfo( GSM_Read_Undefined, 2, 
		       NO_PREDEFINED_PARAMS, NON_LISTABLE );
  FuncObj->SetParamInfo( GSM_Read_Undefined, 0, "input", porINPUT );
  FuncObj->SetParamInfo( GSM_Read_Undefined, 1, "x", porUNDEFINED, 
			NO_DEFAULT_VALUE, PASS_BY_REFERENCE );



  gsm->AddFunction( FuncObj );


}



