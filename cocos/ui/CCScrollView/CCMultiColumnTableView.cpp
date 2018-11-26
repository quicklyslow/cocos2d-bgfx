#include "cocos2d.h"
#include "CCMultiColumnTableView.h"
#include "base/CCTouch.h"

NS_CC_EXT_BEGIN

MultiColumnTableView* MultiColumnTableView::create(Size size)
{
	return MultiColumnTableView::create(nullptr, Vec2(0,0), size, Size(10,10), nullptr);
}

MultiColumnTableView* MultiColumnTableView::create(MultiColumnTableViewDataSource* dataSource, Vec2 p, Size size, Size cellSize)
{
    return MultiColumnTableView::create(dataSource, p, size, cellSize, nullptr);
}

MultiColumnTableView* MultiColumnTableView::create(MultiColumnTableViewDataSource* dataSource, Vec2 p, Size size, Size cellSize, Node *container)
{
    MultiColumnTableView *table = new MultiColumnTableView();
    table->initWithViewSize(size, container);
    table->autorelease();
	table->setDataSource(dataSource);
	table->setStartPosition(p);
    table->setCellSize(cellSize);

    return table;
}

bool MultiColumnTableView::initWithViewSize(Size size, Node* container/* = nullptr*/)
{
    if (ScrollView::initWithViewSize(size,container))
    {
		CC_SAFE_DELETE(_indices);
        _indices		= new std::set<ssize_t>();
        _vordering      = VerticalFillOrder::BOTTOM_UP;
        this->setDirection(Direction::VERTICAL);

        ScrollView::setDelegate(this);
        return true;
    }
    return false;
}


MultiColumnTableView::MultiColumnTableView()
: _touchedCell(nullptr)
, _vordering(VerticalFillOrder::TOP_DOWN)
, _indices(nullptr)
, _dataSource(nullptr)
, _tableViewDelegate(nullptr)
, _oldDirection(Direction::NONE)
, m_colCount(1)
, m_canScroll(true)
, _isUsedCellsDirty(false)
{

}

MultiColumnTableView::~MultiColumnTableView()
{
	CC_SAFE_DELETE(_indices);
}

void MultiColumnTableView::setVerticalFillOrder(VerticalFillOrder fillOrder)
{
    if (_vordering != fillOrder) 
	{
        _vordering = fillOrder;
		if (!_cellsUsed.empty())
		{
			this->reloadData();
		}
    }
}

VerticalFillOrder MultiColumnTableView::getVerticalFillOrder()
{
    return _vordering;
}

void MultiColumnTableView::reloadData()
{
    _oldDirection = Direction::NONE;
    for(const auto &cell : _cellsUsed)
    {
		if(_tableViewDelegate != nullptr) 
		{
			_tableViewDelegate->tableCellWillRecycle(this, cell);
		}

		cell->reset();
		cell->setVisible(false);
		_cellsFreed.pushBack(cell);
	}

    _indices->clear();
	_cellsUsed.clear();

    this->_updateCellPositions();
    this->_updateContentSize();
    if (_dataSource->numberOfCellsInTableView(this) > 0)
    {
        this->scrollViewDidScroll(this);
    }
}

void MultiColumnTableView::refreshData()
{
	for(const auto &cell : _cellsUsed)
	{
		if(_tableViewDelegate != nullptr) 
		{
			_tableViewDelegate->tableCellRefresh(this, cell);
		}
	}
}

TableViewCell *MultiColumnTableView::cellAtIndex(ssize_t idx)
{
	if (_indices->find(idx) != _indices->end())
	{
		for (const auto& cell : _cellsUsed)
		{
			if (cell->getIdx() == idx)
			{
				return cell;
			}
		}
	}

	return nullptr;
}

void MultiColumnTableView::updateCellAtIndex(ssize_t idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }
    ssize_t uCountOfItems = _dataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }

    TableViewCell* cell = this->cellAtIndex(idx);
    if (cell)
    {
        this->_moveCellOutOfSight(cell);
    }
    cell = _dataSource->tableCellAtIndex(this, idx);
	cell->setVisible(true);
    this->_setIndexForCell(idx, cell);
    this->_addCellIfNecessary(cell);
}

void MultiColumnTableView::insertCellAtIndex(ssize_t idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }

    ssize_t uCountOfItems = _dataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }

	ssize_t newIdx = 0;

	auto cell = cellAtIndex(idx);
	if (cell)
	{
		newIdx = _cellsUsed.getIndex(cell);
		// Move all cells behind the inserted position
		for (ssize_t i = newIdx; i < _cellsUsed.size(); i++)
		{
			cell = _cellsUsed.at(i);
			this->_setIndexForCell(cell->getIdx()+1, cell);
		}
	}

    //insert a new cell
    cell = _dataSource->tableCellAtIndex(this, idx);
	cell->setVisible(true);
    this->_setIndexForCell(idx, cell);
    this->_addCellIfNecessary(cell);

    this->_updateCellPositions();
    this->_updateContentSize();
}

void MultiColumnTableView::removeCellAtIndex(ssize_t idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }

    ssize_t uCountOfItems = _dataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }

    ssize_t newIdx = 0;

    TableViewCell* cell = this->cellAtIndex(idx);
    if (!cell)
    {
        return;
    }

    newIdx = _cellsUsed.getIndex(cell);

    //remove first
    this->_moveCellOutOfSight(cell);

    /*this->_updateCellPositions();
//    [_indices shiftIndexesStartingAtIndex:idx+1 by:-1];

	for (ssize_t i=_cellsUsed->count()-1; i > newIdx; i--)
    {
        cell = (CCMultiColumnTableViewCell*)_cellsUsed->objectAtIndex(i);
        this->_setIndexForCell(cell->getIdx()-1, cell);
    }
	if (m_cellCount > 0)
	{
		this->scrollViewDidScroll(this);
	}*/
	this->reloadData();
}

TableViewCell *MultiColumnTableView::dequeueCell()
{
	TableViewCell *cell;

	if (_cellsFreed.empty()) {
		cell = NULL;
	} else {
		cell = _cellsFreed.at(0);
		cell->retain();
		_cellsFreed.erase(0);
		cell->autorelease();
	}
	return cell;
}

void MultiColumnTableView::_addCellIfNecessary(TableViewCell * cell)
{
    if (cell->getParent() != this->getContainer())
    {
        this->getContainer()->addChild(cell);
    }
    _cellsUsed.pushBack(cell);
    _indices->insert(cell->getIdx());
    _isUsedCellsDirty = true;
}

void MultiColumnTableView::_updateContentSize()
{
    Size size = Size::ZERO;

	ssize_t cellsCount = _dataSource->numberOfCellsInTableView(this);

	Size& cellSize = m_cellBaseSize;
	if (m_cellSizes.size() != 0)
	{
		cellSize = m_cellSizes.back();
	}
	
    if (cellsCount > 0)
    {
        switch (this->getDirection())
        {
			case Direction::HORIZONTAL:
                size = Size(_vCellsPositions[cellsCount].x + cellSize.width, _viewSize.height);
                break;
            default:
                size = Size(_viewSize.width, _vCellsPositions[cellsCount].y + cellSize.height);
                break;
        }
    }

    this->setContentSize(size);

	if (_oldDirection != _direction)
	{
		if (_direction == Direction::HORIZONTAL)
		{
			this->setContentOffset(Vec2(0,0), false);
		}
		else
		{
			this->setContentOffset(Vec2(0,this->minContainerOffset().y), false);
		}
		_oldDirection = _direction;
	}

}

Vec2 MultiColumnTableView::_offsetFromIndex(ssize_t index)
{
    Vec2 offset = this->__offsetFromIndex(index);

    const Size cellSize = _dataSource->tableCellSizeForIndex(this, index);
	
    if (_vordering == VerticalFillOrder::TOP_DOWN)
    {
        offset.y = this->getContainer()->getContentSize().height - offset.y - cellSize.height;
    }
    return offset;
}

Vec2 MultiColumnTableView::__offsetFromIndex(ssize_t index)
{
    Vec2 offset;
    Size  cellSize;

    switch (this->getDirection())
    {
		case Direction::HORIZONTAL:
            offset = _vCellsPositions[index];
            break;
        default:
            offset = _vCellsPositions[index];
            break;
    }

    return offset;
}

ssize_t MultiColumnTableView::_indexFromOffset(Vec2 offset, bool bStartIdx)
 {
    ssize_t index = CC_INVALID_INDEX;
    const ssize_t maxIdx = _dataSource->numberOfCellsInTableView(this) - 1;

    if (_vordering == VerticalFillOrder::TOP_DOWN)
    {
        offset.y = this->getContainer()->getContentSize().height - offset.y;
    }
    index = this->__indexFromOffset(offset, bStartIdx);
    if (index != CC_INVALID_INDEX)
    {
        index = MAX(0, index);
        if (index > maxIdx)
        {
            index = CC_INVALID_INDEX;
        }
    }

    return index;
}

ssize_t MultiColumnTableView::indexFromOffset(Vec2 offset)
{
	ssize_t index = CC_INVALID_INDEX;
	const ssize_t cellCount = _dataSource->numberOfCellsInTableView(this);
    const ssize_t maxIdx = cellCount - 1;
	Size& cellSize = m_cellBaseSize;

    if (_vordering == VerticalFillOrder::TOP_DOWN)
    {
        offset.y = this->getContainer()->getContentSize().height - offset.y;
    }
	for (ssize_t i = 0; i < cellCount; i++)
	{
		if (_dataSource != nullptr)
		{
			cellSize = _dataSource->tableCellSizeForIndex(this, i);
		}
		
		if (offset.x >= _vCellsPositions[i].x && offset.x <= _vCellsPositions[i].x+cellSize.width)
		{
			if(offset.y >= _vCellsPositions[i].y && offset.y <= _vCellsPositions[i].y+cellSize.height)
			{
				index = i;
				break;
			}
		}
	}

    if (index != CC_INVALID_INDEX)
    {
        index = MAX(0, index);
        if (index > maxIdx)
        {
            index = CC_INVALID_INDEX;
        }
    }

    return index;
}

ssize_t MultiColumnTableView::__indexFromOffset(Vec2 offset, bool bStartIdx)
{
    ssize_t low = 0;
    ssize_t high = _dataSource->numberOfCellsInTableView(this)/m_colCount - 1;
    if (high < 0) {
        return CC_INVALID_INDEX;
    }
	
    float search;
    switch (this->getDirection())
    {
		case Direction::HORIZONTAL:
            search = offset.x;
            break;
        default:
            search = offset.y;
            break;
    }

    while (high >= low)
    {
        ssize_t index = low + (high - low) / 2;
        float cellStart = 0.0f;
		float cellEnd = 0.0f;
		switch(this->getDirection())
		{
		case Direction::HORIZONTAL:
			cellStart = _vCellsPositions[index*m_colCount].x;
			cellEnd = _vCellsPositions[(index + 1)*m_colCount].x;
			break;
		default:
			cellStart = _vCellsPositions[index*m_colCount].y;	
			cellEnd = _vCellsPositions[(index + 1)*m_colCount].y;
			break;
		}

        if (search >= cellStart && search <= cellEnd)
        {
			if (bStartIdx)
			{
				return index*m_colCount;
			}
			else
			{
				return (index+1)*m_colCount;
			}
        }
        else if (search < cellStart)
        {
            high = index - 1;
        }
        else
        {
            low = index + 1;
        }
    }

    if (low <= 0) {
        return 0;
    }

    return CC_INVALID_INDEX;
}

void MultiColumnTableView::_moveCellOutOfSight(TableViewCell *cell)
{
	if(_tableViewDelegate != nullptr) {
		_tableViewDelegate->tableCellWillRecycle(this, cell);
	}
	_cellsFreed.pushBack(cell);
	_cellsUsed.eraseObject(cell);
    _isUsedCellsDirty = true;
    
    _indices->erase(cell->getIdx());
    cell->reset();
	cell->setVisible(false);
}

void MultiColumnTableView::_setIndexForCell(ssize_t index, TableViewCell *cell)
{
    cell->setAnchorPoint(Vec2(0.0f, 0.0f));
    cell->setPosition(this->_offsetFromIndex(index));
    cell->setIdx(index);
}

void MultiColumnTableView::_updateCellPositions() {
    ssize_t cellsCount = _dataSource->numberOfCellsInTableView(this);
    _vCellsPositions.resize(cellsCount + 1);

    if (cellsCount > 0)
    {
        Vec2 currentPos = m_startPos;
        Size & cellSize = m_cellBaseSize;
		ssize_t index = 0;
		ssize_t pos = 0;
		
        for (ssize_t i = 0; i < cellsCount; i++)
        {
			_vCellsPositions[i] = currentPos;

			if(_dataSource != nullptr)
			{
				cellSize = _dataSource->tableCellSizeForIndex(this, i);
			}

            switch (this->getDirection())
            {
                case Direction::HORIZONTAL:
					{
						ssize_t rowCount = m_colCount;
						index = (i+1)/rowCount;
						pos = (i+1)%rowCount;
						if (pos == 0)
						{
							currentPos.x += (m_margin.width + cellSize.width);
						}
						currentPos.y = m_startPos.y + (pos)*(m_margin.height + cellSize.height);
					}
                    break;
				default:
					{
						ssize_t colCount = m_colCount;
						index = (i+1)/colCount;
						pos = (i+1)%colCount;
						currentPos.x = m_startPos.x + (pos)*(m_margin.width + cellSize.width);
						if (pos == 0)
						{
							currentPos.y +=  (m_margin.height + cellSize.height);
						}
					}
					break;
            }
        }
        _vCellsPositions[cellsCount] = _vCellsPositions[cellsCount-1];//1 extra value allows us to get right/bottom of the last cell
    }

}

void MultiColumnTableView::scrollViewDidScroll(ScrollView* view)
{
    ssize_t uCountOfItems = _dataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems)
    {
        return;
    }
    
    if (_isUsedCellsDirty)
    {
        _isUsedCellsDirty = false;
        std::sort(_cellsUsed.begin(), _cellsUsed.end(), [](TableViewCell *a, TableViewCell *b) -> bool{
            return a->getIdx() < b->getIdx();
        });
    }

	if(_tableViewDelegate != nullptr) {
		_tableViewDelegate->scrollViewDidScroll(this);
	}

    ssize_t startIdx = 0, endIdx = 0, idx = 0, maxIdx = 0;
    Vec2 offset = this->getContentOffset() * -1;
    maxIdx = MAX(uCountOfItems-1, 0);

    if (_vordering == VerticalFillOrder::TOP_DOWN)
    {
		offset.x += m_startPos.x;
        offset.y = offset.y + _viewSize.height/this->getContainer()->getScaleY() - m_startPos.y;
    }
    startIdx = this->_indexFromOffset(offset, true);
	if (startIdx == CC_INVALID_INDEX)
	{
		startIdx =  m_colCount*((uCountOfItems-1)/m_colCount);
	}

    if (_vordering == VerticalFillOrder::TOP_DOWN)
    {
        offset.y -= _viewSize.height/this->getContainer()->getScaleY();
    }
    else
    {
        offset.y += _viewSize.height/this->getContainer()->getScaleY();
    }
    offset.x += _viewSize.width/this->getContainer()->getScaleX();

    endIdx  = this->_indexFromOffset(offset, false);
    if (endIdx == CC_INVALID_INDEX)
	{
		endIdx = uCountOfItems - 1;
	}

    if (!_cellsUsed.empty())
    {
		auto cell = _cellsUsed.at(0);
		idx = cell->getIdx();

        while(idx <startIdx)
        {
            this->_moveCellOutOfSight(cell);
            if (!_cellsUsed.empty())
            {
                cell = _cellsUsed.at(0);
                idx = cell->getIdx();
            }
            else
            {
                break;
            }
        }
    }
    if (!_cellsUsed.empty())
    {
		auto cell = _cellsUsed.back();
		idx = cell->getIdx();

        while(idx <= maxIdx && idx > endIdx)
        {
            this->_moveCellOutOfSight(cell);
			if (!_cellsUsed.empty())
			{
				cell = _cellsUsed.back();
				idx = cell->getIdx();
            }
            else
            {
                break;
            }
        }
    }

    for (ssize_t i=startIdx; i <= endIdx; i++)
    {
        if (_indices->find(i) != _indices->end())
        {
            continue;
        }
        this->updateCellAtIndex(i);
    }
}

void MultiColumnTableView::scrollViewEndScroll(ScrollView* view)
{
	if(_tableViewDelegate != nullptr) {
		_tableViewDelegate->scrollViewEndScroll(this);
	}
}

void MultiColumnTableView::onTouchEnded(Touch *pTouch, Event *pEvent)
{
    if (!this->isVisible()) {
        return;
    }

    

    ScrollView::onTouchEnded(pTouch, pEvent);
	if (_touchedCell){
		Rect bb = this->getBoundingBox();
		bb.origin = _parent->convertToWorldSpace(bb.origin);

		if (bb.containsPoint(pTouch->getLocation()) && _tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
			_tableViewDelegate->tableCellTouched(this, _touchedCell);
		}

		_touchedCell = nullptr;
	}
}

bool MultiColumnTableView::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    if (!this->isVisible()) {
        return false;
    }

    bool touchResult = ScrollView::onTouchBegan(pTouch, pEvent);

    if(_touches.size() == 1)
	{
        ssize_t        index;
        Vec2           point;

        point = this->getContainer()->convertTouchToNodeSpace(pTouch);

        index = this->indexFromOffset(point);
		if (index == CC_INVALID_INDEX)
		{
			_touchedCell = nullptr;
		}
        else
		{
			_touchedCell  = this->cellAtIndex(index);
            _touchedCell->setTouchPoint(pTouch->getLocation());
		}

        if (_touchedCell && _tableViewDelegate != NULL)
		{
            _tableViewDelegate->tableCellHighlight(this, _touchedCell);
        }
    }
    else if(_touchedCell) 
	{
		if(_tableViewDelegate != NULL)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
		}

		_touchedCell = nullptr;
    }

    return touchResult;
}

void MultiColumnTableView::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	if (!m_canScroll)
	{
		if (_touchedCell)
		{
			Vec2 point = this->getContainer()->convertTouchToNodeSpace(pTouch);

			ssize_t index = this->indexFromOffset(point);
			if (index == CC_INVALID_INDEX)
			{
				if(_tableViewDelegate != nullptr)
				{
					_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
				}
				_touchedCell = nullptr;
			}
		}
		return;
	}
	
    ScrollView::onTouchMoved(pTouch, pEvent);


	if (_touchedCell) 
	{
		if (isTouchMoved())
		{
			Vec2 point = _touchedCell->convertTouchToNodeSpace(pTouch);
			if(_tableViewDelegate != nullptr)
			{
				_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
			}
			_touchedCell = nullptr;
		}
		else
		{
			/*Vec2 point = this->getContainer()->convertTouchToNodeSpace(pTouch);

			ssize_t index = this->indexFromOffset(point);
			if (index != _touchedCell->getIdx())
			{
				_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
				_touchedCell = NULL;
			}*/
		}
	}
}

void MultiColumnTableView::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
	ScrollView::onTouchCancelled(pTouch, pEvent);

    if (_touchedCell)
	{
		if(_tableViewDelegate != NULL)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
		}

		_touchedCell = NULL;
    }
}


void MultiColumnTableView::setStartPosition(Vec2 const& point)
{
	m_startPos = point;
}

void MultiColumnTableView::setMargin(Size const& margin)
{
	m_margin = margin;
}

void MultiColumnTableView::setCellSize(Size const& size)
{
	m_cellBaseSize = size;
}

void MultiColumnTableView::pushCellSizes(Size const& size)
{
	m_cellSizes.push_back(size);
}

void MultiColumnTableView::setColCount(ssize_t cols)
{
    m_colCount = cols;
    
    if (this->getDirection() == Direction::BOTH) {
        this->_updateContentSize();
    }
}

void MultiColumnTableView::setScrollEnabled(bool enable)
{
	m_canScroll = enable;
}

NS_CC_EXT_END

