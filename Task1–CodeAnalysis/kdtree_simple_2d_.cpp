// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// (c) 2014 Stefan Romberg, sromberg@web.de
//
// License: Proprietary. Not for re-distribution. Neither for commercial nor
//          non-commercial use. Do not use in production code.
//          This piece of code was created for educational purposes only.
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#include "kdtree_simple_2d_.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <climits>
#include <cstring>
#include <algorithm>
#include <vector>

// for k-nn search
#include <ffkdtree/bounded_priority_queue.hpp>

// =============================================================================
namespace kdtree_example
{

// VM: pass out vars by reference or return in a tuple to avoid null pointer access.
static void minmax_xy(const Points2d& P,
                      float* minx, float* maxx,
                      float* miny, float* maxy)
{
	float minX = FLT_MAX, minY = FLT_MAX;
	float maxX = -FLT_MAX, maxY = -FLT_MAX;
	for(int i=0; i<P.size(); i++)
	{
		const IdxPt2d& p = P[i];
		if( p.x < minX )
			minX = p.x;
		if( p.x > maxX )
			maxX = p.x;
		if( p.y < minY )
			minY = p.y;
		if( p.y > maxY )
			maxY = p.y;
	}
	*minx = minX;
	*maxx = maxX;
	*miny = minY;
	*maxy = maxY;
}

static void split(const Points2d& P, int splitIdx, Points2d& P_left, Points2d& P_right)
{
    // VM: use static_cast<>. Or better just const auto or size_type. Cast not necessary.
	const int numPts = (int) P.size();
	
	if( numPts == 0 )
		return;
	
	P_left.resize(splitIdx);
	
	int remaining = numPts-splitIdx;
	P_right.resize(remaining);
	
    // VM: it seems std::vector<Point> would more clearly express what Points2d actually is.
    // We could use vector's move semantics to avoid copying (if algorithm permits)
    // Raw pointer memory copying is error-prone.

    // VM: If still a copy is needed, use iterators and STL.
    std::copy(P.begin(), P.begin() + splitIdx, P_left.begin());
    std::copy(P.begin() + splitIdx, P.end(), P_right.begin());

	memcpy(&(P_left[0]), &(P[0]), splitIdx*sizeof(IdxPt2d));
	memcpy(&(P_right[0]), &(P[splitIdx]), remaining*sizeof(IdxPt2d));
}

// =============================================================================

KdTreeSimple2d_::~KdTreeSimple2d_()
{
    // VM: this indicates ownership of pointers. Better to use unique_ptr<>.
	if( v_left != NULL )
		delete v_left;
	if( v_right != NULL )
		delete v_right;
}

// =============================================================================
// creation
// =============================================================================

KdTreeSimple2d_* KdTreeSimple2d_::build(Points2d P /* copy */)
{
	return build(P, 0);
}

KdTreeSimple2d_* KdTreeSimple2d_::build(Points2d& P, const int depth)
{
	const size_t numPts = P.size();
	
	if( numPts == 1 )
	{
        // VM: using move sematics we could return this temp object by value
        // and pass to a move ctor for ownership transfer.
		// return a leaf storing this point
		return new KdTreeSimple2d_(P[0]);
	}
	else
	{
		size_t medianIdx = numPts >> 1;
		float splitVal;
		if( ((depth & 1) == 0) ) { // is even depth
			assert( (depth % 2) == 0 ); // VM: this is always true.
			std::sort(P.begin(), P.end(), OrderByX()); // VM: also can use C++11 lambdas.
			splitVal = P[medianIdx].x;
		} else {
			assert( (depth % 2) != 0 );
			std::sort(P.begin(), P.end(), OrderByY());
			splitVal = P[medianIdx].y;
		}
		
        // VM: split() could return a tuple of Points2d objects.
        // Move semantics would allow moving new objects from the function.
		Points2d P_left, P_right;
		split(P, (int) medianIdx, P_left, P_right);
		
		float minX, maxX, minY, maxY;
		minmax_xy(P, &minX, &maxX, &minY, &maxY);

		Region2d r(minX, minY, maxX, maxY);
		if( depth == 0 ){
			r = Region2d(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX); // whole 2D space
		}

        // Could be returned by value to avoid working with pointers.
        // Move semantics/RVO would optimize out possible copying.
		KdTreeSimple2d_* v = new KdTreeSimple2d_();
		v->region   = r;
		v->splitVal = splitVal;
		v->v_left   = build(P_left, depth+1);
		v->v_right  = build(P_right, depth+1);
		return v;
	}
}

// =============================================================================
// range search
// =============================================================================

// VM: below methods traverse the tree in DFS order. This strategy can be extracted.
// Particular actions (report, search) could be extracted to a visitor strategy.
// Traversal strategy might be non-recursive to avoid possible stack overflow.

void KdTreeSimple2d_::reportSubTree(Points2d& pts /* out */) const
{
	if( v_left == NULL ){ // is leaf
		pts.push_back(pt);
	} else {
		v_left->reportSubTree(pts);
		v_right->reportSubTree(pts);
	}
}

void KdTreeSimple2d_::rangeSearch(const Region2d& R_query, Points2d& pts) const
{
    // VM: use nullptr.
	if( v_left == NULL ){ // is leaf
		if( R_query.contains(pt) )
			pts.push_back(pt);
	} else {

		if( R_query.contains(region) || R_query.overlap(region) ){
			v_left->_rangeSearch(R_query, pts);
		}

		if( R_query.contains(region) || R_query.overlap(region) ){
			v_right->_rangeSearch(R_query, pts);
		}
	}
}

void KdTreeSimple2d_::reportSubTree(vector<int>& indexes /* out */) const
{
	if( v_left == NULL ){ // is leaf
		indexes.push_back(pt.idx);
	} else {
		v_left->reportSubTree(indexes);
		v_right->reportSubTree(indexes);
	}
}

// =============================================================================
// radius search
// =============================================================================

void KdTreeSimple2d_::radiusSearch(const float x, const float y, const float radius, Points2d& found) const
{
	assert( radius > 0 );
	
	Region2d queryRegion(x-radius, y-radius, x+radius, y+radius);
	
	Points2d pts;
	pts.reserve(found.capacity());
	rangeSearch(queryRegion, pts);
	
	found.reserve(found.size()+pts.size());
	const size_t numPoints2d = pts.size();
	for(size_t j=0; j<numPoints2d; j++)
	{
		const IdxPt2d& pt = pts[j];
		float dx = x - pt.x;
		float dy = y - pt.y;
		
		if( sqrtf(dx*dx + dy*dy) <= radius ){
			found.push_back(pt);
		}
	}
}

// VM: this method mostly duplicates the above one. Simple fix would be to set default minDist = 0.
// Better fix -- extract comparison strategy, namely, the predicate to consider point 'found'.
void KdTreeSimple2d_::radiusSearch(const float x, const float y, const float minDist, const float maxDist, Points2d& found) const
{
	assert( minDist >= 0 );
	assert( maxDist > minDist );
	
	Region2d queryRegion(x-maxDist, y-maxDist, x+maxDist, y+maxDist);
	
	Points2d pts;
	pts.reserve(found.capacity());
	rangeSearch(queryRegion, pts);
	
	found.reserve(found.size()+pts.size());
	const int numPoints2d = (int) pts.size();
	for(int j=0; j<numPoints2d; j++)
	{
		const IdxPt2d& pt = pts[j];
		float dx = x - pt.x;
		float dy = y - pt.y;
		float dist = sqrtf(dx*dx + dy*dy);
		
		if( dist <= minDist && dist >= maxDist ){
			found.push_back(pt);
		}
	}
}

// =============================================================================
// knn search
// =============================================================================

void KdTreeSimple2d_::_nnSearch(float q_x, float q_y, IdxPt2d& best_node, float& min_dist, int depth, int& visitedNodes) const
{
	++visitedNodes;
	//--------------------------------------------------------------------------
	if( isLeaf() ){
	//--------------------------------------------------------------------------
		// compute true distance to current node
		float dx   = q_x - pt.x;
		float dy   = q_y - pt.y;
		float dist = sqrtf(dx*dx + dy*dy); // IMPORTANT: Take square root for proper checking of split values later!
		
		// update current nearest neighbor
		if( dist < min_dist ){
			min_dist  = dist;
			best_node = pt;
		}
		//printf("[%3d]  > Leaf reached! pt.idx: %d\n", visitedNodes, pt.idx);
		//--------------------------------------------------------------------------
	} else {
		//--------------------------------------------------------------------------
		float q_cd_val			= ( (depth % 2) == 0 ) ? q_x : q_y;
		bool  search_first_left = q_cd_val < splitVal;
		
		if( search_first_left ){
			if( (q_cd_val - min_dist) <= splitVal ){
				v_left->_nnSearch(q_x, q_y, best_node, min_dist, depth+1, visitedNodes);
			}
			if( (q_cd_val + min_dist) > splitVal ){
				v_right->_nnSearch(q_x, q_y, best_node, min_dist, depth+1, visitedNodes);
			} 
		} else {
			if( (q_cd_val + min_dist) > splitVal ){
				v_right->_nnSearch(q_x, q_y, best_node, min_dist, depth+1, visitedNodes);
			}
			if( (q_cd_val - min_dist) <= splitVal ){
				v_left->_nnSearch(q_x, q_y, best_node, min_dist, depth+1, visitedNodes);
			}
		}
	}
}

void KdTreeSimple2d_::nnSearch(float x, float y, IdxPt2d& found, float& distance) const
{
	distance         = FLT_MAX;
	int depth        = 0;
	int visitedNodes = 0;
	
	_nnSearch(x, y, found, distance, depth, visitedNodes);
}

struct NnSearchEntry
{
	IdxPt2d pt;
	float	dist;
	
    // VM: delete default copy-constructor if not in use.
	NnSearchEntry(){}
	NnSearchEntry(const IdxPt2d& point, float distance) : pt(point), dist(distance){}
	bool operator < (const NnSearchEntry& rhs) const { return dist < rhs.dist; }
};

// VM: last parameter should be some interface or specific type.
// Logic mostly duplcates _nnSearch(). Extracting found node handling to a strategy
// would allow to define search logic once.
void KdTreeSimple2d_::_knnSearch(float q_x, float q_y, int knn, int depth, 
                                 int& visitedNodes, Points2d& found, void* vpq) const
{
	++visitedNodes;
	
    // VM: this C-style cast uses 'extra information' about vpq type.
	bounded_priority_queue<NnSearchEntry>& pq = *(bounded_priority_queue<NnSearchEntry>*)(vpq);
	//--------------------------------------------------------------------------
	if( isLeaf() ){
		//--------------------------------------------------------------------------
		// compute true distance to current node
		float dx   = q_x - pt.x;
		float dy   = q_y - pt.y;
		float dist = sqrtf(dx*dx + dy*dy); // IMPORTANT: Take square root for proper checking of split values later!
		
		// update nearest neighbor queue
		if( pq.size() < knn || dist < pq.bottom().dist ){
			pq.push( NnSearchEntry(pt,dist) );
		}
		//printf("[%3d]  > Leaf reached! pt.idx: %d\n", visitedNodes, pt.idx);
		//--------------------------------------------------------------------------
	} else { // run recursion on tree branches but only if the distance to the 
		//          cell is smaller than to the worst neighbor in the queue
		//--------------------------------------------------------------------------
		float q_cd_val			= ( (depth % 2) == 0 ) ? q_x : q_y;
		bool  search_first_left = q_cd_val < splitVal;
		
		if( search_first_left ){
			if( pq.size() < knn || (q_cd_val - pq.bottom().dist) <= splitVal ){
				v_left->_knnSearch(q_x, q_y, knn, depth+1, visitedNodes, found, vpq);
			}
			if( pq.size() < knn || (q_cd_val + pq.bottom().dist) > splitVal ){
				v_right->_knnSearch(q_x, q_y, knn, depth+1, visitedNodes, found, vpq);
			} 
		} else {
			if( pq.size() < knn || (q_cd_val + pq.bottom().dist) > splitVal ){
				v_right->_knnSearch(q_x, q_y, knn, depth+1, visitedNodes, found, vpq);
			}
			if( pq.size() < knn || (q_cd_val - pq.bottom().dist) <= splitVal ){
				v_left->_knnSearch(q_x, q_y, knn, depth+1, visitedNodes, found, vpq);
			}
		}
	}
}

void KdTreeSimple2d_::knnSearch(float x, float y, int knn, Points2d& found, vector<float>& distances) const
{
	assert( knn >= 1 );
	
	IdxPt2d best_node;
	int depth        = 0;
	int visitedNodes = 0;

	bounded_priority_queue<NnSearchEntry> pq(knn);
	_knnSearch(x, y, knn, depth, visitedNodes, found, &pq);
	
	while( pq.size() > 0 )
	{
		NnSearchEntry& nn = pq.top();
		found.push_back(nn.pt);
		distances.push_back(nn.dist);
		pq.pop_top();
	}
}

}
