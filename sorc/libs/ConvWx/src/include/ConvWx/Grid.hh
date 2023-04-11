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
 * @file  Grid.hh
 * @brief Simple 2 dimensional data grid with extra formatting specifications
 *        used for input and output
 * @class  Grid
 * @brief Simple 2 dimensional data grid with extra formatting specifications
 *        used for input and output
 *
 * Grid has a GridData base class, where GridData has the data (the values,
 * and dimensions). This derived class adds a name and units, scaling,
 * encoding and compression options.  The default encoding is Grid::INT8,
 * the default scaling is Grid::DYNAMIC (chosen automatically based on
 * data content), and the default compression state is true (gzip).
 *
 * Encoding, scaling and compression may not make sense for some 
 * implementations, and can be ignored.  In this implementation the
 * values are used in the InterfaceLL class
 */

# ifndef    GRID_HH
# define    GRID_HH

#include <string>
#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/GridData.hh>

//----------------------------------------------------------------
class Grid : public GridData
{
public:

  /**
   * @enum Encoding_t
   * @brief The data encodings (encoding is used for input/output)
   */
  typedef enum
  {
    ENCODING_INT8,       /**< Each data element is 8 bits */
    ENCODING_INT16,      /**< Each data element is 16 bits */
    ENCODING_FLOAT32     /**< Each data element is a 32 bit float */
  } Encoding_t;

  /**
   * @enum Scaling_t
   * @brief The data scaling options (scaling is used for input/output)
   */
  typedef enum
  {
    SCALING_DYNAMIC,    /**< The data is converted between floating point
			 *   and encoded values based on the range of floating
			 *   point values */
    SCALING_SPECIFIED   /**< The data is converted between floating point
			 *   and encoded values using fixed scale and bias */
  } Scaling_t;

  /**
   * All empty constructor
   */
  Grid(void);

  /**
   * Constructor that sets particular data values into the Grid
   *
   * @param[in] name  Name of the grid
   * @param[in] units  Units of the grid
   * @param[in] data  An array of values with assumed GridData 2 dimensional
   *                  ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   */
  Grid(const std::string &name, const std::string &units, const double *data,
       const int nx, const int ny, const double missing);

   /**
   * Constructor that sets particular data values into the Grid
   *
   * @param[in] name  Name of the grid
   * @param[in] units  Units of the grid
   * @param[in] data  An array of values with assumed GridData 2 dimensional
   *                  ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] nz  Z number of points
   * @param[in] missing  The data missing value
   */
  Grid(const std::string &name, const std::string &units, const double *data,
       const int nx, const int ny, const int nz, const double missing);
  
  /**
   * Constructor that sets particular data values into the Grid, with vector input
   *
   * @param[in] name  Name of the grid
   * @param[in] units  Units of the grid
   * @param[in] data  An vector of values with assumed GridData 2 dimensional
   *                  ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   */
  Grid(const std::string &name, const std::string &units,
       const std::vector<double> &data,
       const int nx, const int ny, const double missing);

  /**
   * Constructor that sets all data values to missing
   *
   * @param[in] name   Name of the grid
   * @param[in] units  Units of the grid
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   */
  Grid(const std::string &name, const std::string &units,
       const int nx, const int ny, const double missing);
  
   /**
   * Constructor that sets all data values to missing
   *
   * @param[in] name   Name of the grid
   * @param[in] units  Units of the grid
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] nz  Z number of pointsq
   * @param[in] missing  The data missing value
   */
  Grid(const std::string &name, const std::string &units,
       const int nx, const int ny, const int nz, const double missing);

  /**
   * Constructor that makes a copy of an input Grid, but changes the name
   * and units
   *
   * @param[in] g  An existing Grid
   * @param[in] name  Name to give the constructed grid
   * @param[in] units Units to give the constructed grid
   */
  Grid(const Grid &g, const std::string &name, const std::string &units);
  
  /**
   * Constructor that using input GridData, setting name and units
   *
   * @param[in] name  Name to give the constructed grid
   * @param[in] units Units to give the constructed grid
   * @param[in] g  An existing GridData
   */
  Grid(const std::string &name, const std::string &units, const GridData &g);
  
  /**
   * Constructor that makes a copy of an input Grid, but changes name to
   * one of two inputs, and sets all the data values to missing.
   *
   * @param[in] g  An existing Grid
   * @param[in] name   Name to give the constructed Grid (unless empty)
   * @param[in] backupName  Name to give constructed grid when name is empty
   * @param[in] units  Units to give the constructed grid
   *
   * @note  The created object is given all missing data values
   */
  Grid(const Grid &g, const std::string &name, const std::string &backupName,
       const std::string &units) ;

  /**
   * Copy constructor, shallow copy does not work
   *
   * @param[in] g  Object to copy in
   */
  Grid(const Grid &g);
  
  /**
   * Destructor
   */
  virtual ~Grid(void);

  /**
   * Operator=, shallow operator= does not work
   *
   * @param[in] g  Object to copy in
   *
   * @return reference to the local object
   */
  Grid & operator=(const Grid &g);

  /**
   * Operator==
   *
   * @param[in] g  Object to compare
   *
   * @return true for equality
   */
  bool operator==(const Grid &g) const;

  /**
   * Change the name 
   *
   * @param[in] name  Name to use
   */
  inline void changeName(const std::string &name) {pName = name;}

  /**
   * Change the units
   *
   * @param[in] units  Units to change local grid's units to
   */
  inline void changeUnits(const std::string &units) {pUnits = units;}

  /**
   * Change name and units, but only if the input name is not empty
   *
   * @param[in] name  A name or the empty string
   * @param[in] units  Units to use
   *
   * If name is the empty string, the method has no effect
   */
  void changeNameAndUnits(const std::string &name, const std::string &units);

  /**
   * @return  Grid name
   */
  inline std::string getName(void) const {return pName;}

  /**
   * @return  Grid units
   */
  inline std::string getUnits(void) const {return pUnits;}

  /**
   * @return  One line description of the Grid
   */
  std::string sprint(void) const;

  /**
   * Log a one line description of the Grid as returned from sprint().
   */
  void logGrid(void) const;

  /**
   * Log a one line description of the Grid as returned from sprint() 
   * using the input severity
   *
   * @param[in] severity  Severity measure to use.
   */
  void logGrid(const LogStream::Log_t severity) const;

  /**
   * Set scaling to Grid::SCALING_DYNAMIC
   */
  void setScalingDynamic(void);

  /**
   * Set scaling to Grid::SCALING_SPECIFIED
   *
   * @param[in] scale  Value for scale
   * @param[in] bias   Value for bias
   *
   */
  void setScalingSpecified(const double scale, const double bias);

  /**
   * Set the encoding type
   *
   * @param[in] encoding  Encoding choice
   */
  void setEncoding(const Encoding_t encoding);

  /**
   * Set grid compression on or off 
   *
   * @param[in] status  Value to use
   */
  void setCompression(const bool status);

  /**
   * @return Grid encoding
   */
  inline Encoding_t getEncoding(void) const {return pEncoding;}

  /**
   * @return Grid scaling type
   */
  inline Scaling_t getScaling(void) const {return pScaling;}

  /**
   * return the Grid scale and bias values
   *
   * @param[out] scale  Grid scale
   * @param[out] bias  Grid bias
   */
  inline void getScaleBias(double &scale, double &bias) const
  {
    scale = pScale;
    bias = pBias;
  }

  /**
   * @return Grid compression status
   */
  inline bool getCompressionStatus(void) const {return pCompression;}

  /**
   * Filter the Grid forcing integer valued grid values in the range 0 to
   * DIGITAL_MAX (see ConvWxConstants.hh)
   *
   * @note rounds and caps the original values into the desired range
   */
  void digitalFilter(void);

  /**
   * Filter the Grid, replacing non-finite values with the missing data value
   */
  void filterNans(void);

protected:
  
  
  std::string pName;   /**< Description of the data  (field name) */
  std::string pUnits;  /**< Description of the data  (units) */
  Encoding_t pEncoding;/**< Encoding for output */
  Scaling_t pScaling;  /**< Scaling for output*/
  double pScale;       /**< Scale factor for the case of 
			*   pScaling = Grid::SCALING_DYNAMIC */
  double pBias;        /**< Bias for the case of 
			*   pScaling = Grid::SCALING_DYNAMIC */
  bool pCompression;   /**< True for gzip compression */

private:  

  /**
   * Set state to have default values:  pEncoding = Grid::ENCODING_INT8,
   * pScaling = Grid::SCALING_DYNAMIC
   */
  void pSetDefaultScalingAndEncoding(void);
};

# endif     // GRID_HH 
