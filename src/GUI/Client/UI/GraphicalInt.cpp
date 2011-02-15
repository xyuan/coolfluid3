// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <QDoubleSpinBox>
#include <QVariant>
#include <QVBoxLayout>

#include <climits>

#include "Math/MathConsts.hpp"

#include "GUI/Client/UI/UintSpinBox.hpp"

#include "GUI/Client/UI/GraphicalInt.hpp"

using namespace CF::GUI::ClientUI;

GraphicalInt::GraphicalInt(bool isUint, CF::Common::Option::ConstPtr opt, QWidget * parent)
  : GraphicalValue(parent),
    m_isUint(isUint)
{
  m_spinBox = new QDoubleSpinBox(/*isUint, */this);

  if(m_isUint)
    m_spinBox->setRange(Math::MathConsts::Uint_min(), Math::MathConsts::Uint_max());
  else
    m_spinBox->setRange(Math::MathConsts::int_min(), Math::MathConsts::int_max());

  m_layout->addWidget(m_spinBox);

  m_spinBox->setDecimals(0);

//  m_spinBox->setValue(12.34)s;

  if(opt.get() != nullptr)
  {
    if(isUint)
      this->setValue(opt->value<CF::Uint>());
    else
      this->setValue(opt->value<int>());
  }
  connect(m_spinBox, SIGNAL(valueChanged(double)), this, SLOT(integerChanged(double)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

GraphicalInt::~GraphicalInt()
{
  delete m_spinBox;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool GraphicalInt::setValue(const QVariant & value)
{
  if(value.canConvert(m_isUint ? QVariant::UInt : QVariant::Int))
  {
    m_originalValue = value;
    m_spinBox->setValue(m_isUint ? value.toUInt() : value.toInt());
    return true;
  }

  return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QVariant GraphicalInt::value() const
{
  if(m_isUint)
    return (Uint) m_spinBox->value();

  return (int) m_spinBox->value();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void GraphicalInt::integerChanged(double value)
{
  emit valueChanged();
}
