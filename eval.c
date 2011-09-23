#include "myLisp.h"

VALUE eval(VALUE tree, VALUE env)
{
  VALUE v;
  if (DIRECTVAL_P(tree)) return tree;
  else if (PAIR_P(tree) && SYMBOL_P(CAR(tree))) 
    {

      if(strcmp(SYMBOL_NAME(CAR(tree)), "car") == 0) return procedure_car(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cdr") == 0) return procedure_cdr(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cons") == 0) return procedure_cons(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "eq") == 0) return eq(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "atom") == 0) return atom(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "+") == 0) return add(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "-") == 0) return sub(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "*") == 0) return mul(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "/") == 0) return divide(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "%") == 0) return mod(CDR(tree), env);

      /* special form */
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cond") == 0) return cond(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "lambda") == 0) return lambda(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "quote") == 0) return quote(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "define") == 0) return define(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "define-macro") == 0) return define_macro(CDR(tree), env);
    }
  else if (SYMBOL_P(tree))
    {
      v = assoc(tree, env);
      if (!NIL_P(v)) return CDR(v);
      fprintf(stderr, "Undefined symbol '%s'", SYMBOL_NAME(tree));
      return Qnil;
    }

  return apply(CAR(tree), CDR(tree), env);
}

VALUE apply(VALUE func, VALUE args, VALUE  env)
{
  VALUE fbody = eval(func, env);
  
  if(!DIRECTVAL_P(fbody))
    {
      if(CLOSURE_P(fbody))
	{

	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);

  
	  VALUE lis = args;
	  VALUE eval_lis = Qnil;
	  while(!NIL_P(lis))
	    {
	      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
	      lis = CDR(lis);
	    }
  
	  return eval(e, append(pairlis(params, eval_lis), env));
	}
      else if(MACRO_P(fbody))
	{
	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);
	  VALUE r = eval(e, append(pairlis(params, args), env));

	  return eval(r, env);
	}
    }


    fprintf(stderr, "invalid application ");
    print_tree(cons(func, args));
    return Qnil; 
}

VALUE quote(VALUE args, VALUE env)
{
  return CAR(args);
}

VALUE  procedure_car(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return CAR(CAR(eval_lis));
}

VALUE  procedure_cdr(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return CDR(CAR(eval_lis));
}


VALUE eq(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return (CAR(eval_lis) == CAR(CDR(eval_lis))) ? Qtrue : Qfalse;
}

VALUE atom(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

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
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }
  long acc = FIX2INT(CAR(eval_lis));
  long acct = FIX2INT(CAR(CDR(eval_lis)));

  return INT2FIX(acc % acct);
}


VALUE  procedure_cons(VALUE  args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return cons(CAR(eval_lis), CAR(CDR(eval_lis)));
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
  print_tree(env);
  return CAR(CAR(args));
}

VALUE lambda(VALUE args, VALUE env)
{
  LVALUE *lambda = make_obj(env);
  lambda->u.basic.type = CLOSURE;
  GETPARAMS(lambda) = CAR(args);
  GETE(lambda) = CAR(CDR(args));

  return (VALUE)lambda;
}
