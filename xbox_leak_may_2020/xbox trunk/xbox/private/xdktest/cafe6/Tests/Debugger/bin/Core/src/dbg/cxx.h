
/*********************** FOOCLASS **********************/
class fooclass {
private:
  int private_value;
  int id;
  static int ref_cnt;
public:

  fooclass();
  ~fooclass();

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

  void chain1_func();					// chain1 function
  int chain2_func() {					// chain2 function
  START:
      return private_value+1;
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
