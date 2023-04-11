// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>
/**
 * @file Xml.hh
 * @brief Xml read/write static methods
 * @class Xml
 * @brief Xml read/write static methods
 *
 */

# ifndef    XML_HH
# define    XML_HH

#include <string>
#include <vector>

//----------------------------------------------------------------
class Xml
{
public:

  /**
   * @class Attribute
   * @brief attributes of XML item consisting of name/string value pairs.
   */
  class Attribute
  {
  public:
    /**
     * Empty constructor, member strings are empty
     */
    inline Attribute(void) {};

    /**
     * Constructor copies input arguments to members, in obvious way
     * @param[in] name  Name to use
     * @param[in] val  Value to use
     */
    inline Attribute(const std::string &name, const std::string &val) :
      pName(name), pVal(val) {}

    /**
     * Destructor
     */
    inline virtual ~Attribute(void) {};

    /**
     * @return private member pName
     */
    inline const std::string &getName(void) const { return pName; }

    /**
     * @return private member pVal
     */
    inline const std::string &getVal(void) const { return pVal; }

    /**
     * Sets private member pName to input
     * @param[in] name  Name to use
     */
    inline void setName(const std::string &name) { pName = name; }

    /**
     * Sets private member pVal to input
     * @param[in] val  Value to use
     */
    inline void setVal(const std::string &val) { pVal = val; }
  protected:
  private:

    std::string pName;  /**< The attribute name */
    std::string pVal;   /**< The attribute value */
  };

  /**
   * Empty constructor
   */
  Xml(void);

  /**
   * Empty destructor
   */
  virtual ~Xml(void);

  /**
   * Form and return pairs of strings derived from parsing an XML buffer,
   * looking for tagged strings, each with an optional attribute.
   * The first element in each pair is the string, the second is the 
   * attribute value (or empty if none)
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  Tag  The tag value for the strings
   * @param[in] attribute  The attribute name to look for
   * @param[out] p  the pairs of value/attribute
   *
   * @return true if successful.
   *
   * Example:
   *  - tag = "TAG"
   *  - attribute = "ATR"
   *  - buf = "<TAG>v0</TAG> <TAG ATR=2.0>v1</TAG>"
   *  - p = <"v0',""> <"v1","2.0">
   */
  static bool
  readXmlStringArrayWithAtt(const std::string &buf, const std::string &tag,
			    const std::string &attribute,
			    std::vector<std::pair<std::string,std::string> > &p);

  /**
   * Parse an XML buffer, return the string bound by a particular tag,
   * when the tag can have attributes.  The tag/end tag pairs are included
   * in the returned value.
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  The Tag to look for
   * @param[in] doPrintErrorMsg  True to print error message if the tag is not
   *                             found
   * @param[out] retStr The XML string that is the tagged subset, including the
   *                 tag/end tag
   *
   * @return true for success.
   *
   * Example:
   *  - tag = "TAG"
   *  - buf = "stuff <TAG a0=5 a1="x" a2=7>more stuff</TAG>"
   *  - retStr = "<TAG a0=5 a1="x" a2=7>more stuff</TAG>"
   */
  static bool readXmlStringWithAtt(const std::string &buf,
				   const std::string &tag,
				   const bool doPrintErrorMsg,
				   std::string &retStr);

  /**
   * Parse an XML buffer, returning the string between the first occurrence
   * of a tag and the associated end tag
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  Tag to search for
   * @param[in] doPrintErrorMsg  True to print error message if the tag is not
   *                             found
   * @param[out] retStr  The string or an empty string
   *
   * @return true for success.
   *
   * Example:
   *   - buf = "junk <B>some stuff</B> morejunk"
   *   - tag = "B"
   *   - retStr = "some stuff"
   */
  static bool readXmlString(const std::string &buf, const std::string &tag,
			    const bool doPrintErrorMsg, std::string &retStr);

  /**
   * Parse an XML buffer, returning all strings between a tag/end tag, with
   * warning messages when something is wrong.
   *
   * @param[in] buf  Input buffer
   * @param[in] tag  Tag value
   * @param[out] retStrs  Returned vector of strings
   *
   * @return true for success.
   *
   * Example:
   *  - buf = "stuff <B>27.234</B> <B>junk</B> more stuff"
   *  - tag = B
   *  - retStrs = "<B>27.234</B>", "<B>junk</B>"
   */
  static bool readXmlStringArray(const std::string &buf,
				 const std::string &tag,
				 std::vector<std::string> &retStrs);

  /**
   * Parse an XML buffer, returning all strings between a tag/end tag, with
   * no warning messages when something goes wrong.
   *
   * @param[in] buf  Input buffer
   * @param[in] tag  Tag value
   * @param[out] retStrs  Returned vector of strings
   *
   * @return true for success.
   *
   * Example:
   *  - buf = "stuff <B>27.234</B> <B>junk</B> more stuff"
   *  - tag = B
   *  - retStrs = "<B>27.234</B>", "<B>junk</B>"
   */
  static bool readXmlStringArrayNoWarnings(const std::string &buf,
					   const std::string &tag,
					   std::vector<std::string> &retStrs);

  /**
   * Parse an XML buffer, returning an integer value associated with a tag.
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  Tag to search for
   * @param[in] doPrintErrorMsg  True to print error message if the tag is not
   *                              found
   * @param[out] intV  Integer value that is parsed between the tag and end tag
   *
   * @return true for success.
   *
   * Example:
   *   - buf = "stuff  <B>27</B>"
   *   - tag = "B"
   *   - intV = 27
   */
  static bool readXmlInt(const std::string &buf, const std::string &tag,
			 const bool doPrintErrorMsg, int &intV);

  /**
   * Parse an XML buffer, returning a double value associated with a tag.
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  Tag to search for
   * @param[in] doPrintErrorMsg  True to print error message if the tag is not
   *                             found
   * @param[out] doubleV  Double value that is parsed between the tag and end
   *                      tag
   *
   * @return true for success.
   *
   * Example:
   *   - buf = "stuff  <B>27.1344</B>"
   *   - tag = "B"
   *   - doubleV = 27.1344
   */
  static bool readXmlDouble(const std::string &buf, const std::string &tag,
			    const bool doPrintErrorMsg, double &doubleV);

  /**
   * Parse an XML buffer, returning a boolean value associated with a tag.
   *
   * @param[in] buf  The string to parse
   * @param[in] tag  Tag to search for
   * @param[in] doPrintErrorMsg  True to print error message if the tag is not
   *                             found
   * @param[out] boolV  Boolean value that is parsed between the tag and end tag
   *
   * @return true for success.
   *
   * Example:
   *   - buf = "stuff  <B>true</B>"
   *   - tag = "B"
   *   - boolV = true
   */
  static bool readXmlBoolean(const std::string &buf, const std::string &tag,
			     const bool doPrintErrorMsg , bool &boolV);


  /**
   * Read array of tag buffers - buffers include tags.
   * Does support attributes.
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doPrintErrorMsg  True to log errors, false to be silent
   * @param[out] tagBufArray  The returned buffers
   *
   * @return True for success, false for failure
   */
  static bool readTagBufArray(const std::string &xmlBuf, const std::string &tag,
			      const bool doPrintErrorMsg,
			      std::vector<std::string> &tagBufArray);

  /**
   * Read a tag buffer - buffer includes tags. Does support attributes.
   * Loads buffer with string including start tag and end tag.
   * Search starts at searchStart pos.
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] tagBuf  Returned tag buffer
   * @param[in] searchStart  Starting index into the input xmlBuf
   * @param[out] searchEnd  If searchEnd is not NULL it is set to 
   *                        1 beyond the end of the data for the current tag.
   *
   * @return  True for success, false for failure
   */
  static bool readTagBuf(const std::string &xmlBuf, const std::string &tag,
			 const bool doprintErrorMsg, std::string &tagBuf,
			 size_t searchStart = 0, size_t *searchEnd = NULL);

  /**
   * Read string value from an XML buffer, given a tag. No attributes.
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The string
   *
   * @return true for success, false for failure
   */
  static bool readString(const std::string &xmlBuf,
			 const std::string &tag,
			 const bool doprintErrorMsg,
			 std::string &val);

  /**
   * Read string value and attributes from an XML buffer, given a tag.
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The string
   * @param[out] attributes  The attributes within the tag
   *
   * @return true for success, false for failure
   */
  static bool readString(const std::string &xmlBuf,
			 const std::string &tag,
			 const bool doprintErrorMsg,
			 std::string &val,
			 std::vector<Attribute> &attributes);

  /**
   * Read int value from an XML buffer, given a tag.
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The int
   *
   * @return true for success, false for failure
   */
  static bool readInt(const std::string &xmlBuf, const std::string &tag,
		      const bool doprintErrorMsg, int &val);

  /**
   * Read double from an XML buffer, given a tag.
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The double
   * @return true for success, false for failure
   */
  static bool readDouble(const std::string &xmlBuf, const std::string &tag,
			 const bool doprintErrorMsg, double &val);

  
  /**
   * Read boolean from an XML buffer, given a tag
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The boolean
   * @return true for success, false for failure
   */
  static bool readBoolean(const std::string &xmlBuf, const std::string &tag,
			  const bool doprintErrorMsg, bool &val);


  /**
   * Read time from an XML buffer, given a tag
   *
   * @param[in] xmlBuf  Buffer to read from
   * @param[in] tag  Tag to look for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The time
   * @return true for success, false for failure
   */
  static bool readTime(const std::string &xmlBuf, const std::string &tag,
		       const bool doprintErrorMsg, time_t &val);

  /**
   * Read time from a string in XML format
   *
   * @param[in] valStr  String that should have a time
   * @param[out] val  The time
   * @return true for success, false for failure
   */
  static bool readTime(const std::string &valStr, time_t &val);

  /**
   * Read string attribute from a vector of attributes. Look for a particular
   * named attribute, and return the associated value
   *
   * @param[in] attributes  What to search through
   * @param[in] name  The name to search for
   * @param[in] doprintErrorMsg  True to log errors, false to be silent
   * @param[out] val  The value found
   * @return true for success, false for failure
   */
  static bool readStringAttr(const std::vector<Attribute> &attributes,
			     const std::string &name,
			     const bool doprintErrorMsg,
			     std::string &val);

  /**
   * @name write methods
   * Write a value, with tags, to a string.
   * If a format arg is available, you may specify the sprintf format.
   * Otherwise %d, %ld or %g will be used as appropriate.
   * Return a string with the value written to it.
   * @{
   */

  /**
   * Create an XML string for a string input.
   * @param[in] tag  Tag to use
   * @param[in] level Number of spaces to indent
   * @param[in] val  The string
   * @return the XML 
   */
  static std::string writeString(const std::string &tag, int level,
				 const std::string &val);

  /**
   * Create an XML string for a boolean input
   * @param[in] tag  Tag to use
   * @param[in] level Number of spaces to indent
   * @param[in] val  The boolean
   * @return the XML
   */
  static std::string writeBoolean(const std::string &tag, int level, bool val);


  /**
   * Create an XML string for an int input
   * @param[in] tag  Tag to use
   * @param[in] level Number of spaces to indent
   * @param[in] val  The int
   * @param[in] format  Optional format specification, can be empty
   * @return the XML
   */
  static std::string writeInt(const std::string &tag, int level, int val,
			      const char *format = NULL);

  /**
   * Create an XML string for a double input
   * @param[in] tag  Tag to use
   * @param[in] level Number of spaces to indent
   * @param[in] val  The double
   * @param[in] format  Optional format specification, can be empty
   * @return the XML
   */
  static std::string writeDouble(const std::string &tag, int level, double val,
				 const char *format = NULL);
  
  /**
   * Create an XML string for a time input
   * @param[in] tag  Tag to use
   * @param[in] level Number of spaces to indent
   * @param[in] val  The time
   * @return the XML
   */
  static std::string writeTime(const std::string &tag, int level, time_t val);

  /**
   * @}
   */
  
protected:
private:  

  /**
   * Find tag limits in a buffer. The search starts at searchStart.
   *
   * @param[in] xmlBuf  A buffer
   * @param[in] tag  A tag
   * @param[in] searchStart  Where to start the search in xmlBuf
   * @param[out] startPos Returned beginning position of the start tag.
   * @param[out] endPos  Returned index to one beyond the end of the closing tag
   *
   * @return true for success, false for failure
   */
  static bool pFindTagLimits(const std::string &xmlBuf, const std::string &tag,
				 size_t searchStart, size_t &startPos,
				 size_t &endPos);

  /**
   * Decode attributes from attribute string into a vector.
   * There might be no attributes, which is ok.
   *
   * @param[in] attrStr  Buffer with attributes in it 'name="xxx" name="yyy"'
   * @param[out] attributes  Values parsed out of input
   */
  static void pAttrDecode(const std::string &attrStr,
			  std::vector<Attribute> &attributes);

  /**
   * Scan input string for an integer value
   *
   * @param[in] valStr  String that should have a integer
   * @param[out] val   Value scanned
   * @return true for success, false for failure
   */
  static bool pReadInt(const std::string &valStr, int &val);

  /**
   * Scan input string for a boolean value
   * @param[in] valStr  String that should have a boolean 'true', 'TRUE', etc.
   * @param[out] val   Value scanned
   * @return true for success, false for failure
   */
  static bool pReadBoolean(const std::string &valStr, bool &val);

  /**
   * Scan input string for an double value
   * @param[in] valStr  String that should have a double
   * @param[out] val   Value scanned
   * @return true for success, false for failure
   */
  static bool pReadDouble(const std::string &valStr, double &val);

  /**
   * @return string representation of boolean input
   *
   * @param[in] val  Boolean value to turn into a string
   */
  static std::string pWriteBoolean(bool val);

  /**
   * @return string representation of int input
   *
   * @param[in] val  Int value
   * @param[in] format  An optional format, can be NULL
   */
  static std::string pWriteInt(int val, const char *format);

  /**
   * @return string representation of double input
   *
   * @param[in] val  Double value
   * @param[in] format  An optional format, can be NULL
   */
  static std::string pWriteDouble(double val, const char *format);
  
  /**
   * @return string representation of a time input
   * @param[in] val  Time
   */
  static std::string pWriteTime(time_t val);
};

# endif
