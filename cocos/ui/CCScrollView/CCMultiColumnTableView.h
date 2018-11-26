

#ifndef __CCMULTITABLEVIEWCELL_H__
#define __CCMULTITABLEVIEWCELL_H__

#include "CCScrollView.h"
#include "CCTableViewCell.h"
#include "CCTableView.h"

#include <set>
#include <vector>

NS_CC_EXT_BEGIN
class MultiColumnTableView;
/**
 * Sole purpose of this delegate is to single touch event in this version.
 */
class MultiColumnTableViewDelegate : public ScrollViewDelegate
{
public:
    /**
     * Delegate to respond touch event
     *
     * @param table table contains the given cell
     * @param cell  cell that is touched
     * @js NA
     * @lua NA
     */
    virtual void tableCellTouched(MultiColumnTableView* table, TableViewCell* cell) = 0;

    /**
     * Delegate to respond a table cell press event.
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     * @js NA
     * @lua NA
     */
    virtual void tableCellHighlight(MultiColumnTableView* table, TableViewCell* cell){};

    /**
     * Delegate to respond a table cell release event
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     * @js NA
     * @lua NA
     */
    virtual void tableCellUnhighlight(MultiColumnTableView* table, TableViewCell* cell){};

    /**
     * Delegate called when the cell is about to be recycled. Immediately
     * after this call the cell will be removed from the scene graph and
     * recycled.
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     * @js NA
     * @lua NA
     */
    virtual void tableCellWillRecycle(MultiColumnTableView* table, TableViewCell* cell){};

	virtual void tableCellRefresh(MultiColumnTableView* table, TableViewCell* cell){}

};


/**
 * Data source that governs table backend data.
 */
class MultiColumnTableViewDataSource
{
public:
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MultiColumnTableViewDataSource() {}

    /**
     * cell size for a given index
     *
     * @param idx the index of a cell to get a size
     * @return size of a cell at given index
     */
    virtual Size tableCellSizeForIndex(MultiColumnTableView *table, ssize_t idx) {
        return cellSizeForTable(table);
    };
    /**
     * cell height for a given table.
     *
     * @param table table to hold the instances of Class
     * @return cell size
     */
    virtual Size cellSizeForTable(MultiColumnTableView *table) {
        return Size::ZERO;
    };
    /**
     * a cell instance at a given index
     *
     * @param idx index to search for a cell
     * @return cell found at idx
     */
    virtual TableViewCell* tableCellAtIndex(MultiColumnTableView *table, ssize_t idx) = 0;
    /**
     * Returns number of cells in a given table view.
     *
     * @return number of cells
     */
    virtual ssize_t numberOfCellsInTableView(MultiColumnTableView *table) = 0;

};
/**
 * UITableView counterpart for cocos2d for iphone.
 *
 * this is a very basic, minimal implementation to bring UITableView-like component into cocos2d world.
 *
 */
class MultiColumnTableView : public ScrollView, public ScrollViewDelegate
{
public:
    MultiColumnTableView();
    virtual ~MultiColumnTableView();

	static MultiColumnTableView* create(Size size);

    /**
     * An initialized table view object
     *
     * @param cell start point
     * @param size view size
     * @return table view
     */
    static MultiColumnTableView* create(MultiColumnTableViewDataSource* dataSource, Vec2 p, Size size, Size cellSize = Size(10, 10));
    /**
     * An initialized table view object
     *
     * @param dataSource data source;
     * @param size view size
     * @param container parent object for cells
     * @return table view
     */
    static MultiColumnTableView* create(MultiColumnTableViewDataSource* dataSource, Vec2 p, Size size, Size cellSize, Node *container = nullptr);


    /**
     * determines how cell is ordered and filled in the view.
     */
    void setVerticalFillOrder(VerticalFillOrder order);
    VerticalFillOrder getVerticalFillOrder();


    bool initWithViewSize(Size size, Node* container = nullptr);

	/**
     * data source
     * @js NA
     * @lua NA
     */
    MultiColumnTableViewDataSource* getDataSource() { return _dataSource; }
    /**
     * when this function bound to js or lua,the input params are changed
     * in js:var setDataSource(var jsSource)
     * in lua:local setDataSource()
     * @endcode
     */
    void setDataSource(MultiColumnTableViewDataSource* source) { _dataSource = source; }

	 /**
     * delegate
     * @js NA
     * @lua NA
     */
    MultiColumnTableViewDelegate* getDelegate() { return _tableViewDelegate; }
    /**
     * @code
     * when this function bound to js or lua,the input params are changed
     * in js:var setDelegate(var jsDelegate)
     * in lua:local setDelegate()
     * @endcode
     */
    void setDelegate(MultiColumnTableViewDelegate* pDelegate) { _tableViewDelegate = pDelegate; }

    /**
     * Updates the content of the cell at a given index.
     *
     * @param idx index to find a cell
     */
    void updateCellAtIndex(ssize_t idx);
    /**
     * Inserts a new cell at a given index
     *
     * @param idx location to insert
     */
    void insertCellAtIndex(ssize_t idx);
    /**
     * Removes a cell at a given index
     *
     * @param idx index to find a cell
     */
    void removeCellAtIndex(ssize_t idx);
    /**
     * reloads data from data source.  the view will be refreshed.
     */
    void reloadData();

	void refreshData();
    /**
     * Dequeues a free cell if available. nil if not.
     *
     * @return free cell
     */
    TableViewCell *dequeueCell();

    /**
     * Returns an existing cell at a given index. Returns nil if a cell is nonexistent at the moment of query.
     *
     * @param idx index
     * @return a cell at a given index
     */
    TableViewCell *cellAtIndex(ssize_t idx);
 
	void setStartPosition(Vec2 const& point);

	void setMargin(Size const& margin);

	void setCellSize(Size const& size);

	void pushCellSizes(Size const& size);

	void setColCount(ssize_t cols);

	void setScrollEnabled(bool enable);

	MultiColumnTableView* createCellAtIndex(ssize_t idx);

    virtual void scrollViewDidScroll(ScrollView* view);
    virtual void scrollViewDidZoom(ScrollView* view) {}
    virtual void scrollViewEndScroll(ScrollView* view);

	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent) override;
	virtual void onTouchMoved(Touch *pTouch, Event *pEvent) override;
	virtual void onTouchEnded(Touch *pTouch, Event *pEvent) override;
	virtual void onTouchCancelled(Touch *pTouch, Event *pEvent) override;

protected:

    TableViewCell *_touchedCell;
    /**
     * vertical direction of cell filling
     */
    VerticalFillOrder _vordering;

    /**
     * index set to query the indexes of the cells used.
     */
    std::set<ssize_t>* _indices;

    /**
     * vector with all cell positions
     */
    std::vector<Vec2> _vCellsPositions;
    //NSMutableIndexSet *indices_;
    /**
     * cells that are currently in the table
     */
    Vector<TableViewCell*> _cellsUsed;
    /**
     * free list of cells
     */
    Vector<TableViewCell*> _cellsFreed;

	/**
     * weak link to the data source object
     */
    MultiColumnTableViewDataSource* _dataSource;
    /**
     * weak link to the delegate object
     */
    MultiColumnTableViewDelegate* _tableViewDelegate;

	Direction _oldDirection;

    bool _isUsedCellsDirty;
    
	ssize_t indexFromOffset(Vec2 offset);
    ssize_t _indexFromOffset(Vec2 offset, bool bStartIdx = true);
    ssize_t __indexFromOffset(Vec2 offset, bool bStartIdx = true);

    Vec2 __offsetFromIndex(ssize_t index);
    Vec2 _offsetFromIndex(ssize_t index);

    void _moveCellOutOfSight(TableViewCell *cell);
    void _setIndexForCell(ssize_t index, TableViewCell *cell);
    void _addCellIfNecessary(TableViewCell * cell);

    void _updateCellPositions();

	 /**
     * the maximum number of columns.
     */
	Vec2 m_startPos;
	Size m_margin;
    ssize_t m_colCount;

	Size m_cellBaseSize;

	bool m_canScroll;

	std::vector<Size> m_cellSizes;
public:
    void _updateContentSize();

};

NS_CC_EXT_END

#endif /* __CCMULTITABLEVIEWCELL_H__ */



