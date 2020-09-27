// base class
class base {
public:
	virtual void vf1() { int i = 5; };
	virtual int  vf2() { return 10; };
	virtual int  vf3(int ind) { return ind*2; };
};


/*********************** FOOCLASS **********************/
class fooclass : public base {
private:
  int private_value;
  int id;
  static int ref_cnt;
public:

  fooclass();
  ~fooclass();

  void vf1() { int i = 7; };
  int  vf2(int j) { return j; };

  static int static_func() {
  START:
     return 9;
  END:;
  };

  void clear() {                       // Clear private value
  START:
     fooclass::private_value=0;
  END:;
  };

  int getID() {                       // return ID
  START:
      return id;
  END:;
  };

  int getCount() {                    // return reference_cnt
  START:
      return ref_cnt;
  END:;
  };

  operator int() {                    // return private value
  START:
      return (int)private_value;      // conversion operator
  END:;
  };

  inline void inline_func() {         // inline function
  START:
      private_value++;                // increment private value
  END:;
  };
};


/******************** define function for fooclass ************/
 int fooclass::ref_cnt=1;

  fooclass::fooclass() {              // constructor for fooclass
  START:
    id=ref_cnt;
    ref_cnt++;
    private_value=-2*id;
  END:;
  };

  fooclass::~fooclass() {             // destructor for fooclass
  START:
    ref_cnt--;
  END:;
  };
