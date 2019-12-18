/**
 * @file 
 * @brief Header file for working with triangle mazes.
 *
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @date 2019-12-17
 */

#ifndef PROJ3_H
#define PROJ3_H
/** 
 * @brief Map object used to store informatin about maze.
 */
typedef struct {
    int rows;               /**< number of rows in maze*/
    int cols;               /**< number of columns in maze*/
    unsigned char *cells;   /**< maze cells @note should not be accesed directly, but using helper functions*/
} Map;

/**
 * @brief Bit values of borders in a triangle.
 * */
enum borders { BLEFT=0x1,   /**< Border LEFT*/
               BRIGHT=0x2,  /**< Border RIGHT*/
               BTOP=0x4,    /**< Border TOP*/   
               BBOTTOM=0x4  /**< Border BOTTOM*/
};

/** 
 * @brief Frees memory allocated when loading map.
 *
 * @param [in] map Map object which cells to free.
 *
 * @post Cells in Map object pointed to by @p map will be freed and @p map cells set to NULL.
 */
void free_map(Map *map);

/** 
 * @brief Loads triangle map from @p filename to already allocated @p map object.
 *
 * @param [in] filename File name of file containing the map definition.
 * @param [in, out] map Map object into which to load map from @p filename.
 *
 * @returns 0 when correctly allocated and assigned, otherwise
 *          return non-zero value.
 *
 * @pre @p map must me a valid pointer to Map object.
 * @post On correct assignement allocates memory for cells,
 *       @p map needs to be later freed by #free_map.
 */
int load_map(const char *filename, Map *map);

/** 
 * @brief Checks whether @p map cell on row @p r and col @p c contains @p border type of border.
 *
 * @param [in] map Map object which cells will be accessed.
 * @param [in] r Row index.
 * @param [in] c Column index.
 * @param [in] border Valid border definiton number from #borders.
 *
 * @returns true if cell contains this type of border, otherwise false.
 *
 * @pre @p map must me a valid pointer to Map object.
 * @pre @p r and @p c should be in bounds of allocated cells in @p map.
 */
bool isborder(Map *map, int r, int c, int border);

/**
 * @brief Checks whether cell at given row and column contains bottom border.
 *        Wrapper around #isborder with additional rotation check.
 *
 * @param [in] r Row index.
 * @param [in] c Column index.
 *
 * @returns true if cell contains this type of border, otherwise false.
 *
 * @pre @p r and @p c should be in bounds of allocated cells in @p map.
 */
bool hasbottom(int r, int c);

/**
 * @brief Decides which border was crossed as first when entering triangle maze.
 *
 * Decides upon these rules (for righthand algorithm):
 *  1. #BRIGHT when entering maze from left on odd row
 *  2. #BBOTTOM when entering maze from left on even row
 *  3. #BLEFT when entering maze from top
 *  4. #BRIGHT when entering maze from bottom
 *  5. #BTOP when entering maze from right, if cell contains #BTOP
 *  6. #BLEFT when entering maze from right, if cell contains #BBOTTOM.
 *
 *  Order of precedence 1,2,5,6,3,4.
 *  Respectively for lefthand algorithm, except adequately switch right, left, top and bottom.
 * 
 * @param [in] map Map object which cells will be accessed.
 * @param [in] r Row index.
 * @param [in] c Column index.
 * @param [in] leftright Decides which algorithm to use when solving maze.
 *
 * @return one of cell border definitions.
 *
 * @pre @p map must me a valid pointer to Map object.
 * @pre @p r and @p c should be in bounds of allocated cells in @p map.
 */
int start_border(Map *map, int r, int c, int leftright);

/**
 * @brief Tests whether given Map object contains valid maze definition.
 * 
 * Maze map is valid when cell borders are shared with its neighbors and maze
 * is exactly the size of allocated cells in #map.
 *
 * @param [in] map Map object which cells will be tested.
 *
 * @returns 0 in case maze is valid, otherwise returns non-zero value.
 *
 * @pre @p map must me a valid pointer to Map object.
 */
int check_map(Map *map);

/**
 * @brief Loads given filename maze definition into @p map and tests it later.
 *
 * Combination of #load_map and #check_map.
 *
 * @param [in] filename File name of file containing the map definition.
 * @param [in, out] map Map object into which to load map from @p filename.
 *
 * @returns 0 when correctly allocated, assigned and maze @p filename contains valid
 *          maze definition, otherwise returns non-zero value.
 *
 * @pre @p map must me a valid pointer to Map object.
 * @post On correct assignement allocates memory for cells,
 *       @p map needs to be later freed by #free_map.
 */
int load_and_check_map(const char *filename, Map *map);

/**
 * @brief Check whether @p r and/or @p c is out of bounds of cells in @p map.
 *
 * @param [in] map Map object which cells range will be compared to @p r and @p c.
 * @param [in] r Row index to compare with rows in @p map.
 * @param [in] c Column index to compare with cols in @p map.
 *
 * @returns true in case @p r and/or @p c is out of bounds of @p map cells,
 *          otherwise false.
 *
 * @pre @p map must me a valid pointer to Map object.
 */
bool is_out(Map *map, int r, int c);

/**
 * @brief Solves maze using @p leftright algorithm and prints out the used path on stdio.
 *
 * @param [in] map Map object which cells will be accessed.
 * @param [in] r Row index.
 * @param [in] c Column index.
 * @param [in] leftright Decides which algorithm to use when solving maze.
 *
 * @pre @p map must me a valid pointer to Map object.
 * @pre @p r and @p c should be in bounds of allocated cells in @p map.
 */
void print_path(Map *map, int r, int c, int leftright);

#endif
