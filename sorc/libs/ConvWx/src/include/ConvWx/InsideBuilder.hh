/**
 * @file InsideBuilder.hh
 * @brief algorithm for GridData to see if you are inside/outside of 
 *        a closed edge
 * @class InsideBuilder
 * @brief algorithm for GridData to see if you are inside/outside of 
 *        a closed edge
 *
 * Takes an input EdgeFinder, defines a subset of the grid, then evaluates
 * using a 'crossing' algorithm (inside, outside, maybe).
 */

# ifndef INSIDE_BUILDER_H
# define INSIDE_BUILDER_H

class EdgeFinder;

//------------------------------------------------------------------
class InsideBuilder 
{
 public:

  /**
   * Constructor
   *
   * @param[in] e Edgebuilder, with its state filled in.
   *
   * @note the constructor does pretty much everything to identify
   * points that are on the inside.
   */
  InsideBuilder(const EdgeFinder &e);

  /**
   * Destructor
   */
  virtual ~InsideBuilder(void);

  /**
   * @return number of gridpoints (x) in the subset of the grid being evaluated
   */
  inline int nx(void) const {return pNx;}

  /**
   * @return number of gridpoints (y) in the subset of the grid being evaluated
   */
  inline int ny(void) const {return pNy;}

  /**
   * @return true if input point is an interior point 
   *
   * @param[in] x  index into the subset indices
   * @param[in] y  index into the subset indices
   */
  bool isInside(const int x, const int y) const;

protected:
private:

  /**
   * @enum inside_t
   * Classification of points
   */
  typedef enum
  {
    OUT = 0,         /**< Point is not inside */
    IN = 1,          /**< Point is for sure inside */
    MAYBE = 2        /**< NOt sure at this point */
  } Inside_t;

  int pNx;  /**< Subset grid number of points x */
  int  pNy; /**< Subset grid number of points y */

  bool *pIsInside;  /**< * 2d array of dimension nx*ny */

  int pX0; /**< lower left corner of subset grid in full grid */
  int pY0; /**< lower left corner of subset grid in full grid */
  int pX1; /**< upper right corner of subset grid in full grid */
  int pY1; /**< upper right corner of subset grid in full grid */


  /**
   * @return 1d equivalent pointer for use in pIsInside array
   *
   * @param[in] x
   * @param[in] y
   */
  int pPtr(const int x, const int y) const;

};

# endif
