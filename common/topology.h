// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _TOPOLOGY_INCLUDED
#define _TOPOLOGY_INCLUDED

#include <iostream>
#include "geometry.h"

using namespace std;

// *************************************************************
//    TOPOLOGY
// *************************************************************

struct vertex_t;

// an unoriented triangle with its three neighbors and 3 vertices
//          vtx[1]
//           o 
//           | \ -> ngh[1]
// ngh[2] <- |   o vtx[0]
//           | / -> ngh[0]
//           o
//         vtx[2]
struct tri {
  tri *ngh [3];
  vertex_t *vtx [3];
  intT id;
  bool initialized;
  char bad;  // used to mark badly shaped triangles
  void setT(tri *t1, tri *t2, tri* t3) {
    ngh[0] = t1; ngh[1] = t2; ngh[2] = t3; }
  void setV(vertex_t *v1, vertex_t *v2, vertex_t *v3) {
    vtx[0] = v1; vtx[1] = v2; vtx[2] = v3; }
  int locate(tri *t) {
    for (int i=0; i < 3; i++)
      if (ngh[i] == t) return i;
    cout<<"did not locate back pointer in triangulation\n";
    abort(); // did not find
  }
  void update(tri *t, tri *tn) {
    for (int i=0; i < 3; i++)
      if (ngh[i] == t) {ngh[i] = tn; return;}
    cout<<"did not update\n";
    abort(); // did not find
  }
};

// a vertex_t pointing to an arbitrary triangle to which it belongs (if any)
struct vertex_t {
  typedef point2d pointT;
  point2d pt;
  tri *t;
  tri *badT;
  intT id;
  intT reserve;
  void print() {
    cout << id << " (" << pt.x << "," << pt.y << ") " << endl;
  }
  vertex_t(point2d p, intT i) : pt(p), id(i), reserve(-1)
			   , badT(NULL)
  {}
};

inline std::ostream& operator<<(std::ostream& os, const tri& t) {
  return os << "#" << t.id << "(" << t.vtx[0]->pt << ", " << t.vtx[1]->pt << ", " << t.vtx[2]->pt << ")";
}

inline void printNgh(tri& t) {
  if (t.ngh[0] != NULL) cout << *t.ngh[0] << ", ";
  else cout << "NULL, ";
  if (t.ngh[1] != NULL) cout << *t.ngh[1] << ", ";
  else cout << "NULL, ";
  if (t.ngh[2] != NULL) cout << *t.ngh[2] << endl;
  else cout << "NULL" << endl;
}

inline int mod3(int i) {return (i>2) ? i-3 : i;}

// a simplex is just an oriented triangle.  An integer (o)
// is used to indicate which of 3 orientations it is in (0,1,2)
// If boundary is set then it represents the edge through t.ngh[o],
// which is a NULL pointer.
struct simplex {
  tri *t;
  int o;
  bool boundary;
  simplex(tri *tt, int oo) : t(tt), o(oo), boundary(0) {}
  simplex(tri *tt, int oo, bool _b) : t(tt), o(oo), boundary(_b) {}
  simplex(vertex_t *v1, vertex_t *v2, vertex_t *v3, tri *tt) {
    t = tt;
    t->ngh[0] = t->ngh[1] = t->ngh[2] = NULL;
    t->vtx[0] = v1; v1->t = t;
    t->vtx[1] = v2; v2->t = t;
    t->vtx[2] = v3; v3->t = t;
    o = 0;
    boundary = 0;
  }

  void print() {
    if (t == NULL) cout << "NULL simp" << endl;
    else {
      cout << "vtxs=";
      for (int i=0; i < 3; i++) 
	if (t->vtx[mod3(i+o)] != NULL)
	  cout << t->vtx[mod3(i+o)]->id << " (" <<
	    t->vtx[mod3(i+o)]->pt.x << "," <<
	    t->vtx[mod3(i+o)]->pt.y << ") ";
	else cout << "NULL ";
      cout << endl;
    }
  }

  simplex across() {
    tri *to = t->ngh[o];
    if (to != NULL) return simplex(to,to->locate(t));
    else return simplex(t,o,1);
  }

  // depending on initial triangle this could be counterclockwise
  simplex rotClockwise() { return simplex(t,mod3(o+1));}

  bool valid() {return (!boundary);}
  bool isTriangle() {return (!boundary);}
  bool isBoundary() {return boundary;}
  
  vertex_t *firstVertex() {return t->vtx[o];}

  bool inCirc(vertex_t *v) {
    if (boundary || t == NULL) return 0;
    return inCircle(t->vtx[0]->pt, t->vtx[1]->pt, 
		    t->vtx[2]->pt, v->pt);
  }

  // the angle facing the across edge
  double farAngle() {
    return angle(t->vtx[mod3(o+1)]->pt,
		 t->vtx[o]->pt,
		 t->vtx[mod3(o+2)]->pt);
  }

  bool outside(vertex_t *v) {
    if (boundary || t == NULL) return 0;
    return counterClockwise(t->vtx[mod3(o+2)]->pt, v->pt, t->vtx[o]->pt);
  }

  // flips two triangles and adjusts neighboring triangles
  void flip() { 
    simplex s = across();
    int o1 = mod3(o+1);
    int os1 = mod3(s.o+1);

    tri *t1 = t->ngh[o1];
    tri *t2 = s.t->ngh[os1];
    vertex_t *v1 = t->vtx[o1];
    vertex_t *v2 = s.t->vtx[os1];

    t->vtx[o]->t = s.t;
    t->vtx[o] = v2;
    t->ngh[o] = t2;
    if (t2 != NULL) t2->update(s.t,t);
    t->ngh[o1] = s.t;

    s.t->vtx[s.o]->t = t;
    s.t->vtx[s.o] = v1;
    s.t->ngh[s.o] = t1;
    if (t1 != NULL) t1->update(t,s.t);
    s.t->ngh[os1] = t;
  }

  // splits the triangle into three triangles with new vertex_t v in the middle
  // updates all neighboring simplices
  // ta0 and ta0 are pointers to the memory to use for the two new triangles
  void split(vertex_t* v, tri* ta0, tri* ta1) {
    v->t = t;
    tri *t1 = t->ngh[0]; tri *t2 = t->ngh[1]; tri *t3 = t->ngh[2];
    vertex_t *v1 = t->vtx[0]; vertex_t *v2 = t->vtx[1]; vertex_t *v3 = t->vtx[2];
    t->ngh[1] = ta0;        t->ngh[2] = ta1;
    t->vtx[1] = v;
    ta0->setT(t2,ta1,t);  ta0->setV(v2,v,v1);
    ta1->setT(t3,t,ta0);  ta1->setV(v3,v,v2);
    if (t2 != NULL) t2->update(t,ta0);      
    if (t3 != NULL) t3->update(t,ta1);
    v2->t = ta0;
  }

  // splits one of the boundaries of a triangle to form two triangles
  // the orientation dictates which edge to split (i.e., t.ngh[o])
  // ta is a pointer to memory to use for the new triangle
  void splitBoundary(vertex_t* v, tri* ta) {
    int o1 = mod3(o+1);
    int o2 = mod3(o+2);
    if (t->ngh[o] != NULL) {
      cout << "simplex::splitBoundary: not boundary" << endl; abort();}
    v->t = t; // not sure why this is necessary
    tri *t2 = t->ngh[o2];
    vertex_t *v1 = t->vtx[o1]; vertex_t *v2 = t->vtx[o2];
    t->ngh[o2] = ta;   t->vtx[o2] = v;
    ta->setT(t2,NULL,t);  ta->setV(v2,v,v1);
    if (t2 != NULL) t2->update(t,ta);
    v2->t = t; // not sure why this is so
  }

  // splits the triangle on the opposite side
  inline void splitEdgeSecondPart(vertex_t* v, tri* tt, tri* tb) {
    // find direction to caller
    intT o0 = 0;
    for (; o0 < 3; ++ o0) {
      if (tt->ngh[o0] == t) break;
    }
    int o1 = mod3(o0+1);
    int o2 = mod3(o0+2);

    vertex_t *v2 = tt->vtx[o0];
    vertex_t *v4 = tt->vtx[o1];
    vertex_t *v0 = tt->vtx[o2];
    tri *t3 = tt->ngh[o1];

    tt->ngh[o0] = tb-1;
    tt->ngh[o1] = tb;
    tt->vtx[o0] = v;

    tb->setT(t,t3,tt);
    tb->setV(v2,v4,v);

    if (t3 != NULL) t3->update(tt,tb);
  }

  tri* splitEdge(vertex_t* v, tri* ta) {
    int o1 = mod3(o+1);
    int o2 = mod3(o+2);
    if (t->ngh[o] == NULL) {
      cout << "simplex::splitEdge: is boundary" << endl; abort();}

    vertex_t *v0 = t->vtx[o];
    vertex_t *v1 = t->vtx[o1];
    vertex_t *v2 = t->vtx[o2];
    tri *t2 = t->ngh[o1];
    tri *tt = t->ngh[o];

    // set steiner point
    v->pt.x = (v2->pt.x + v0->pt.x)/2;
    v->pt.y = (v2->pt.y + v0->pt.y)/2;

    t->ngh[o] = ta+1;
    t->ngh[o1] = ta;
    t->vtx[o] = v;

    ta->setT(t2,t,tt);
    ta->setV(v1,v,v0);

    if (t2 != NULL) t2->update(t,ta);

    splitEdgeSecondPart(v, tt, ta+1);
    return tt;
  }

  // given a vertex_t v, extends a boundary edge (t.ngh[o]) with an extra 
  // triangle on that edge with apex v.  
  // ta is used as the memory for the triangle
  simplex extend(vertex_t* v, tri* ta) {
    if (t->ngh[o] != NULL) {
      cout << "simplex::extend: not boundary" << endl; abort();}
    t->ngh[o] = ta;
    ta->setV(t->vtx[o], t->vtx[mod3(o+2)], v);
    ta->setT(NULL,t,NULL);
    v->t = ta;
    return simplex(ta,0);
  }

};

// this might or might not be needed
void topologyFromTriangles(triangles<point2d> Tri, vertex_t** vr, tri** tr);

#endif // _TOPOLOGY_INCLUDED
