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
 * @file MultiGrid.hh
 * @brief zero or more Grid objects
 * @class MultiGrid
 * @brief zero or more Grid objects
 */

# ifndef    MULTIGRID_HH
# define    MULTIGRID_HH

#include <vector>
#include <ConvWx/Grid.hh>

//----------------------------------------------------------------
class MultiGrid 
{
public:

  /**
   * Empty constructor, zero Grid objects
   */
  MultiGrid(void);

  /**
   * Constructor for one Grid object.
   * @param[in] g  Grid to use
   *
   * On exit, the MultiGrid consists of one Grid equivalent to input g.
   */
  MultiGrid(const Grid &g);

  /**
   * Destructor
   */
  virtual ~MultiGrid(void);

  /**
   * @return true if the input dimensions are the same as the local MultiGrid
   *        Grid dimensions
   *8
   * @param[in] nx  Grid dimension x
   * @param[in] ny  Grid dimension y
   */
  bool dimensionsOk(const int nx, const int ny) const;

  /**
   * @return true if input object has same number of fields with same name
   * in same order, false otherwise
   * @param[in] g  Grids to compare
   */
  bool fieldsMatch(const MultiGrid &g) const;
  
  /**
   * Append a Grid to the state
   * @param[in] g  Grid to append
   *
   * @note does not check if input Grid dimensions are consistent with local
   *       dimensions, or if a grid with this name is already present
   */
  inline void append(const Grid &g) { pGrids.push_back(g);}

  /**
   * Append a Grid to the state
   * @param[in] g  Grid to append
   *
   * @note does not check if input Grid dimensions are consistent with local
   *       dimensions, but does check if name already present and
   *       if so does not append, with warning message.
   */
  void push_back(const Grid &g);

  /**
   * Append at Grid to the state, giving the Grid a particular name
   * unless the input name is empty, in which case nothing happens. (no append)
   *
   * @param[in] g  Grid to append to state
   * @param[in] name  Name to give appended grid, can be empty.
   *
   * @note does not check if input Grid dimensions are consistent with local
   *       dimensions
   */
  void appendUnlessNoName(const Grid &g, const std::string &name);

  /**
   * Return a copy of the local object 
   *
   * @param[out] m  Object to copy into
   */
  void copyOut(MultiGrid &m) const;

  /*
   * @return Number of Grid objects in the local state
   */
  inline int num(void) const {return static_cast<int>(pGrids.size());}

  /**
   * @return names of the grids
   */
  std::vector<std::string> fieldNames(void) const;

  /**
   *@return a const pointer to a particular Grid.
   *
   *@param[in] i  Index to a Grid, 0, 1, 2, ...
   *
   *@note Does not check if i is in bounds.
   */
  inline const Grid *ithConstGrid(const int i) const {return &pGrids[i];}

  /**
   * @return a pointer to a particular Grid
   *
   *@param[in] i  Index to a Grid, 0, 1, 2, ...
   *
   *@note Does not check if i is in bounds.
   */
  inline Grid *ithGrid(const int i) {return &pGrids[i];}

  /**
   * @return field name of a particular Grid
   *
   *@param[in] i  Index to a Grid, 0, 1, 2, ...
   *
   *@note Does not check if i is in bounds.
   */
  std::string ithGridName(const int i) const;

  /**
   * @return an index to the Grid whose name equals the input field,
   *         or BAD_INDEX for no  matching name
   *
   * @param[in] field  The name to try and match
   */
  int matchingFieldIndex(const std::string &field) const;

  /**
   * @return a const pointer to the Grid whose name equals input fieldName,
   *         or NULL for none.
   *
   * @param[in] fieldname  Name to to try and match
   * @param[in] debugWarn  True to print out a warning when no match is found
   *
   * @note The returned pointer should not be freed.
   */
  const Grid *constGridPtr(const std::string &fieldname,
			   const bool debugWarn = true) const;

  /**
   * @return a pointer to the Grid whose name equals input fieldName,
   *         or NULL for none.
   *
   * @param[in] fieldname  Name to to try and match
   * @param[in] debugWarn  True to print out a warning when no match is found
   *
   * @note The returned pointer should not be freed.
   */
  Grid *gridPtr(const std::string &fieldname, const bool debugWarn = true);

  /**
   * Copy grid data values from one Grid to another internally
   *
   * @param[in] fieldFrom  Name of the Grid to copy from
   * @param[in] fieldTo  Name of the Grid to copy to
   *
   * @return true if found the two Grid objects and did the copy
   */
  bool replaceData(const std::string &fieldFrom, const std::string &fieldTo);

  /**
   * Apply a sx by sy smoothing (averaging) filter to each Grid
   *
   * @param[in] sx  Smoothing window size x
   * @param[in] sy  Smoothing window size y
   * 
   * On exit For each Grid, the data at x,y is the average of all data in the
   *         box bounded by [x-sx,y-sy],  [x+sx,y+sy]
   */
  void smooth(const int sx, const int sy);

  /**
   * Subtract input grid data from local grid, pointwise.
   * Expect same number of fields and same field names for the local and input.
   *
   * @param[in] sub  Grids to subtract
   *
   * @return true if successful
   */
  bool subtract(const MultiGrid &sub);

  /**
   * Add input grids data to local grids, pointwise.
   * Expect same number of fields and same field names for the local and input.
   *
   * @param[in] a  Grids to add
   *
   * @return true if successful
   */
  bool add(const MultiGrid &a);

  /**
   * Modify local grids so at each point the value is <= the input grids
   *
   * @param[in] g  Grids that must be >= local grids everywhere
   *
   * If the individual grids are not same as input grids, no action and
   * an error message.
   */
  void monotonicDecreasing(const MultiGrid &g);

  /**
   * Set each grids encoding to the input
   *
   * @param[in] encoding  Encoding choice
   */
  void setEncoding(const Grid::Encoding_t encoding);


  /**
   * Set each grids scaling to Grid::SCALING_SPECIFIED
   *
   * @param[in] scale  Value for scale
   * @param[in] bias   Value for bias
   */
  void setScalingSpecified(const double scale, const double bias);

  typedef std::vector<Grid>::iterator iterator;
  typedef std::vector<Grid>::const_iterator const_iterator;
  typedef std::vector<Grid>::reverse_iterator reverse_iterator;
  typedef std::vector<Grid>::const_reverse_iterator const_reverse_iterator;

  /**
   * @return iterator begin
   */
  inline iterator       begin()           { return pGrids.begin(); }

  /**
   * @return iterator begin, const
   */
  inline const_iterator begin() const     { return pGrids.begin(); }

  /**
   * @return iterator end
   */
  inline iterator       end()             { return pGrids.end(); }

  /**
   * @return iterator end, const
   */
  inline const_iterator end() const       { return pGrids.end(); }

  /**
   * @return reverse_iterator begin
   */
  inline reverse_iterator rbegin()        { return pGrids.rbegin(); }

  /**
   * @return reverse_iterator begin, const
   */
  inline const_reverse_iterator rbegin() const { return pGrids.rbegin(); }

  /**
   * @return reverse_iterator end
   */
  inline reverse_iterator rend()          { return pGrids.rend(); }

  /**
   * @return reverse_iterator end, const
   */
  inline const_reverse_iterator rend() const { return pGrids.rend(); }

  /**
   * @return size of the pGrids vector
   */
  inline size_t size(void) const       { return pGrids.size(); }

  /**
   * @return true if the pGrids vector is empty
   */
  inline bool empty(void) const        { return pGrids.empty(); }

  /**
   * Reserve space in the pGrids vector
   * @param[in] n  Number of spaces to reserve
   */
  inline void reserve(size_t n)    { pGrids.reserve(n); }

  /**
   * @return reference to a vector element in pGrid
   * @param[in] i  Index
   */
  inline Grid& operator[](size_t i) { return pGrids[i]; }

  /**
   * @return reference to a vector element in pGrid, const
   * @param[in] i  Index
   */
  inline const Grid& operator[](size_t i) const { return pGrids[i]; }

  /**
   * @return reference to the Grid with matching name
   * @param[in] s  Name of grid to search for
   */
  Grid& operator[](const std::string &s);

  /**
   * @return const reference to the Grid with matching name
   * @param[in] s  Name of grid to search for
   */
  const Grid& operator[](const std::string &s) const;

  /**
   * @return iterator to Grid that is equivalent (operator==) to input Grid
   *         or end() if none
   * @param[in] g  Grid to compare to 
   */
  iterator find(const Grid &g);

  /**
   * @return const iterator to Grid that is equivalent (operator==) to input
   *         Grid or end() if none
   * @param[in] g  Grid to compare to 
   */
  const_iterator find(const Grid &g) const;

  /**
   * Erase the vector element refered to by input iterator
   * 
   * @param[in] pos  Iterator to erase
   * @return iterator after the erased one
   */
  iterator erase(iterator pos);

protected:
private:  

  /**
   * The Grid objects
   */
  std::vector<Grid> pGrids;
};

# endif     // MultiGrid_HH
