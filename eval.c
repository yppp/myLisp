#include "myLisp.h"

VALUE eval(VALUE tree, VALUE env)
{
  VALUE v;
  if (DIRECTVAL_P(tree) || CLOSURE_P(tree) || MACRO_P(tree) || NATIVE_PROCEDURE_P(tree)) return tree;
  else if (SYMBOL_P(tree))
    {
      v = assoc(tree, env);
      if (!NIL_P(v)) return CDR(v);
      fprintf(stderr, "Undefined symbol '%s'", SYMBOL_NAME(tree));
      exit(1);
      return Qnil;
    }

  return apply(CAR(tree), CDR(tree), env);
}

VALUE apply(VALUE func, VALUE args, VALUE env)
{
  VALUE fbody = eval(func, env);
  
  if(!DIRECTVAL_P(fbody))
    {
      if(CLOSURE_P(fbody))
	{
	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);
	  VALUE newenv = GETENV(fbody);

	  VALUE eval_lis = evlis(args, env);

	  envpush(env);
	  VALUE ret = eval(e, append(pairlis(params, eval_lis), newenv));
	  envpop();

	  return ret;
	}
      else if(MACRO_P(fbody))
	{
	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);
	  VALUE r = eval(e, append(pairlis(params, args), env));

	  return eval(r, env);
	}
      else if(NATIVE_PROCEDURE_P(fbody))
	{
	  return ((LVALUE*)fbody)->u.native.proc(args, env);
	}
    }


    fprintf(stderr, "invalid application ");
    print_tree(cons(func, args));
    exit(1);
    return Qnil; 
}

VALUE quote(VALUE args, VALUE env)
{
  (void)env;
  return CAR(args);
}

VALUE  procedure_car(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);

  return CAR(CAR(eval_lis));
}

VALUE  procedure_cdr(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);

  return CDR(CAR(eval_lis));
}


VALUE eq(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);

  return (CAR(eval_lis) == CAR(CDR(eval_lis))) ? Qtrue : Qfalse;
}

VALUE atom(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);

  return FIXNUM_P(CAR(eval_lis))
    || NIL_P(CAR(eval_lis))
    || TRUE_P(CAR(eval_lis))
    || FALSE_P(CAR(eval_lis))
    || SYMBOL_P(CAR(eval_lis)) ? Qtrue : Qfalse;
}

VALUE add(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  long acc = 0;

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc += FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE sub(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  long acc = 0;

  evalret = eval(CAR(lis), env);
  acc = FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
  lis = CDR(lis);

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc -= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE mul(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  long acc = 1;

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc *= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE divide(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  long acc;

  evalret = eval(CAR(lis), env);
  acc = FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
  lis = CDR(lis);

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc /= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE mod(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);
  long acc = FIX2INT(CAR(eval_lis));
  long acct = FIX2INT(CAR(CDR(eval_lis)));

  return INT2FIX(acc % acct);
}


VALUE  procedure_cons(VALUE args, VALUE env)
{
  VALUE concon = (VALUE)make_obj(env);
  VALUE eval_lis = evlis(args, env);

  ((LVALUE*)concon)->u.basic.type = CELL;
  ((LVALUE*)concon)->u.cell.car = CAR(eval_lis);
  ((LVALUE*)concon)->u.cell.cdr = CAR(CDR(eval_lis));

  return concon;
}

VALUE cond(VALUE args, VALUE env)
{
  VALUE lis = args;

  VALUE inlis;
  VALUE last_eval = Qnil;

  while(!NIL_P(lis))
    {
      inlis = CAR(lis);
      if(eval(CAR(inlis), env) == Qtrue)
	{
	  inlis = CDR(inlis);
	  while(!NIL_P(inlis))
	    {
	      last_eval = eval(CAR(inlis), env);
	      inlis = CDR(inlis);
	    }
	  break;
	}
      lis = CDR(lis);
    }

  return last_eval;
}

VALUE define(VALUE args, VALUE env)
{
  VALUE ltop = topenv;
  if (NIL_P(ltop)) 
    {
      topenv = cons(cons(CAR(args), eval(CAR(CDR(args)), env)), Qnil);
    }
  else
    {
      topenv = append(cons(cons(CAR(args), eval(CAR(CDR(args)), env)), Qnil), topenv);
    }
  return CAR(args);
}

VALUE define_macro(VALUE args, VALUE env)
{
  LVALUE *macro = make_obj(env);
  macro->u.basic.type = MACRO;
  GETPARAMS(macro) = CDR(CAR(args));
  GETE(macro) = CAR(CDR(args));
  GETENV(macro) = env;

  VALUE ltop = topenv;
  if (NIL_P(ltop)) 
    {
      topenv = cons(cons(CAR(CAR(args)), (VALUE)macro), Qnil);
    }
  else
    {
      while(!NIL_P(CDR(ltop))) ltop = CDR(ltop);

      topenv = append(cons(cons(CAR(CAR(args)), (VALUE)macro), Qnil), topenv);
    }

  return CAR(CAR(args));
}

VALUE lambda(VALUE args, VALUE env)
{
  LVALUE *lambda = make_obj(env);
  lambda->u.basic.type = CLOSURE;
  GETPARAMS(lambda) = CAR(args);
  GETE(lambda) = CAR(CDR(args));
  GETENV(lambda) = env;

  return (VALUE)lambda;
}

VALUE set(VALUE args, VALUE env)
{
  VALUE eval_lis = evlis(args, env);
  VALUE f = assoc(CAR(args), env);


  if(NIL_P(f))
    {
      fprintf(stderr, "symbol not defined");
      exit(1);
    }

  CDR(f) = CAR(CDR(eval_lis));

  return eval(CAR(f), env);
}

VALUE evlis(VALUE list, VALUE env)
{
  VALUE values = Qnil;

  while (!NIL_P(list))
    {
      values = append(values, cons(eval(CAR(list), env), Qnil));
      list = CDR(list);
    }
  return values;
}
