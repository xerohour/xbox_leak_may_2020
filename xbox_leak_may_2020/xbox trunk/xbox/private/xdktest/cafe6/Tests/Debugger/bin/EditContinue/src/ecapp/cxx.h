
/*********************** FOOCLASS **********************/
class fooclass {
private:
  int private_value;
  int id;
  static int ref_cnt;

public:

  fooclass();
  ~fooclass();

  static int static_func() { return 9; };

  void clear() 
  {                       // Clear private value
     fooclass::private_value=0;
  };

  int getID() { return id; };

  int getCount() 
  {                    // return reference_cnt
      return ref_cnt;
  };

  operator int() 
  {                    // return private value
      return (int)private_value;      // conversion operator
  };

  inline void inline_func() 
  {         
      private_value++;                // increment private value
  };

  void chain1_func();				
  
  int chain2_func()
  {				
      return private_value + 2;
  };

};


/******************** define function for fooclass ************/
 int fooclass::ref_cnt=1;

  fooclass::fooclass() 
  {              // constructor for fooclass
    id=ref_cnt;
    ref_cnt++;
    private_value=-2*id;
  };

  fooclass::~fooclass() 
  {             // destructor for fooclass
    ref_cnt--;
  };


class base1
{
protected:
	// inherited member variable.
	int base1_var;
public:
	base1(void){base1_var = 1;}
	int base1_func(int i);
	virtual int base1_virtual_func(int i); // base1
	int base1_overloaded_func(int i); // base1
	int base1_overridden_func(int i); // base1
};

class base2
{
protected:
	int base2_var;
public:
	base2(void){base2_var = 2;}
	// inherited, non-overloaded, non-overridden member function
	int base2_func(int i);
	virtual int base2_virtual_func(int i);
};

class derivedm : public base1, public base2
{
private:
	int derivedm_private_var;
	int derivedm_private_func(int i);

protected:
	int derivedm_protected_var;
	int derivedm_protected_func(int i);

public:
	int derivedm_public_var;
	// class-nested enum.
	enum enum_class {one_class = 1, two_class, three_class};
	// declaration of class-nested enum var.
	enum_class enum_class_var;

	static int derivedm_static_var;
	//static int derivedm_static_func(int i);

	derivedm(void);
	// non-inherited, non-overloaded member function.
	int derivedm_public_func(int i);
	// virtual inherited non-overloaded, overridden member function
	virtual int base1_virtual_func(int i); // derivedm
	// inherited, overloaded, non-overridden member function
	int base1_overloaded_func(char ch); // derivedm
	// inherited, non-overloaded, overridden member function
	int base1_overridden_func(int i); // derivedm
};

