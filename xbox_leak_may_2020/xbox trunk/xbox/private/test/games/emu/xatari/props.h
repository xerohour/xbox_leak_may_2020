#ifndef PROPERTIES_HXX
#define PROPERTIES_HXX

#include "bspf.h"
#include "string1.h"

/**
  This class represents objects which maintain a collection of 
  properties.  A property is a key and its corresponding value.

  A properties object can contain a reference to another properties
  object as its "defaults"; this second properties object is searched 
  if the property key is not found in the original property list.

  @author  Bradford W. Mott
  @version $Id: Props.hxx,v 1.3 1998/08/29 15:29:52 bwmott Exp $
*/
class Properties
{
  public:
    /**
      Creates an empty properties object with the specified defaults.  The 
      new properties object does not claim ownership of the defaults.

      @param defaults The defaults
    */
    Properties(const Properties* defaults = 0);

    /**
      Creates a properties list by copying another one

      @param properties The properties to copy
    */
    Properties(const Properties& properties);

    /**
      Destructor
    */
    virtual ~Properties();

  public:
    /**
      Get the value assigned to the specified key.  If the key does
      not exist then the empty string is returned.

      @param key The key of the property to lookup
      @return The value of the property 
    */
    string get(const string& key) const;

    /**
      Set the value associated with key to the given value.

      @param key The key of the property to set
      @param value The value to assign to the property
    */
    void set(const string& key, const string& value);

  public:
    /**
      Load properties from the specified input stream

      @param in The input stream to use
    */
    void load(istream2& in);
 
    /**
      Save properties to the specified output stream

      @param out The output stream to use
    */
    void save(ostream2& out);

  public:
    /**
      Read the next quoted string from the specified input stream
      and returns it.

      @param in The input stream to use
      @return The string inside the quotes
    */ 
    static string readQuotedString(istream2& in);
     
    /**
      Write the specified string to the given output stream as a 
      quoted string.

      @param out The output stream to use
      @param s The string to output
    */ 
    static void writeQuotedString(ostream2& out, string& s);

  public:
    /**
      Overloaded assignment operator

      @param properties The properties object to set myself equal to
      @return Myself after assignment has taken place
    */
    Properties& operator = (const Properties& properties);

  private:
    /**
      Helper function to perform a deep copy of the specified
      properties.  Assumes that old properties have already been 
      freed.

      @param properties The properties object to copy myself from
    */
    void copy(const Properties& properties);

  private:
    // Structure used for storing properties
    struct Property 
    {
      string key;
      string value;
    };

    // Pointer to properties object to use for defaults or the null pointer
    const Properties* myDefaults;

    // Pointer to a dynamically allocated array of properties
    Property* myProperties;

    // Current capacity of the properties array
    unsigned int myCapacity;

    // Size of the properties array (i.e. the number of <key,value> pairs)
    unsigned int mySize;
};
#endif

