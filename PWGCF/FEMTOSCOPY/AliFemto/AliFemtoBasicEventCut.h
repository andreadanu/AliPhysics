///
/// \file AliFemtoBasicEventCut
///

#ifndef ALIFEMTOBASICEVENTCUT_H
#define ALIFEMTOBASICEVENTCUT_H

#include "AliFemtoEventCut.h"

/// \class AliFemtoBasicEventCut
/// \brief The basic cut for events.
///
/// Only cuts on event multiplicity and z-vertex position
///
class AliFemtoBasicEventCut : public AliFemtoEventCut {
public:

  AliFemtoBasicEventCut(); ///< Default Constructor
  AliFemtoBasicEventCut(AliFemtoBasicEventCut& c);  ///< Copy Constructor
  virtual ~AliFemtoBasicEventCut(); ///< Destructor
  AliFemtoBasicEventCut& operator=(AliFemtoBasicEventCut& c);  ///< Assignment Operator

  void SetEventMult(const int& lo,const int& hi);     ///< Set min and max acceptable event multiplicity
  void SetVertZPos(const float& lo, const float& hi); ///< Set min and max acceptable vertex z-coordinate
  void SetAcceptBadVertex(bool b);  ///< Check if ZDC participants is greater than 1.0
  int NEventsPassed() const;  ///< Number of events passed
  int NEventsFailed() const;  ///< Number of events failed

  bool GetAcceptBadVertex();
  /* bool GetAcceptOnlyPhysics(); */
  void SetTriggerSelection(int trig);  ///< Set the trigger cluster

  void SetEPVZERO(const float& lo, const float& hi); ///< Set the min and max allowed event reaction plane angle

  virtual TList* AppendSettings(TList*, const TString& prefix="") const;
  virtual AliFemtoString Report();
  virtual bool Pass(const AliFemtoEvent* event);

  AliFemtoBasicEventCut* Clone();

private:   // here are the quantities I want to cut on...

  int fEventMult[2];       ///< range of multiplicity
  float fVertZPos[2];      ///< range of z-position of vertex
  float fPsiEP[2];         ///< range of vzero ep angle
  bool fAcceptBadVertex;   ///< Set to true to accept events with bad vertex
  long fNEventsPassed;     ///< Number of events checked by this cut that passed
  long fNEventsFailed;     ///< Number of events checked by this cut that failed
  bool fAcceptOnlyPhysics; ///< Accept only physics events
  int  fSelectTrigger;     ///< If set, only given trigger will be selected

#ifdef __ROOT__
  /// \cond CLASSIMP
  ClassDef(AliFemtoBasicEventCut, 2);
  /// \endcond
#endif

};

inline void AliFemtoBasicEventCut::SetEventMult(const int& lo, const int& hi)
{
  fEventMult[0] = lo;
  fEventMult[1] = hi;
}

inline void AliFemtoBasicEventCut::SetVertZPos(const float& lo, const float& hi)
{
  fVertZPos[0] = lo;
  fVertZPos[1] = hi;
}
inline void AliFemtoBasicEventCut::SetEPVZERO(const float& lo, const float& hi)
{
  fPsiEP[0] = lo;
  fPsiEP[1] = hi;
}

inline int AliFemtoBasicEventCut::NEventsPassed() const
{
  return fNEventsPassed;
}

inline int AliFemtoBasicEventCut::NEventsFailed() const
{
  return fNEventsFailed;
}

inline void AliFemtoBasicEventCut::SetTriggerSelection(int trig)
{
  fSelectTrigger = trig;
}

inline AliFemtoBasicEventCut* AliFemtoBasicEventCut::Clone()
{
  AliFemtoBasicEventCut* c = new AliFemtoBasicEventCut(*this);
  return c;
}

inline AliFemtoBasicEventCut::AliFemtoBasicEventCut(AliFemtoBasicEventCut& c):
  AliFemtoEventCut(c),
  fAcceptBadVertex(c.fAcceptBadVertex),
  fNEventsPassed(0),
  fNEventsFailed(0),
  fAcceptOnlyPhysics(c.fAcceptOnlyPhysics),
  fSelectTrigger(c.fSelectTrigger)
{
  fEventMult[0] = c.fEventMult[0];
  fEventMult[1] = c.fEventMult[1];
  fVertZPos[0] = c.fVertZPos[0];
  fVertZPos[1] = c.fVertZPos[1];
  fPsiEP[0] = c.fPsiEP[0];
  fPsiEP[1] = c.fPsiEP[1];
}

inline AliFemtoBasicEventCut& AliFemtoBasicEventCut::operator=(AliFemtoBasicEventCut& c)
{
  if (this != &c) {
    AliFemtoEventCut::operator=(c);
    fEventMult[0] = c.fEventMult[0];
    fEventMult[1] = c.fEventMult[1];
    fVertZPos[0] = c.fVertZPos[0];
    fVertZPos[1] = c.fVertZPos[1];
    fPsiEP[0] = c.fPsiEP[0];
    fPsiEP[1] = c.fPsiEP[1];
  }

  return *this;
}


#endif
