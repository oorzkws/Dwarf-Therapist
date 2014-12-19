/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "cellcolors.h"
#include "defines.h"
#include "utils.h"
#include "viewcolumn.h"
#include "dwarftherapist.h"

CellColors::CellColors(QObject *parent)
    : QObject(parent)
    , m_override_cell_colors(false)
{
    //connect(DT,SIGNAL(settings_changed()),SLOT(read_settings()));
}

CellColors::CellColors(const CellColors &cc)
    : QObject(cc.parent())
{
    m_colors = cc.m_colors;
    m_override_cell_colors = cc.m_override_cell_colors;
}

void CellColors::load_settings(QSettings &s){
    m_override_cell_colors = s.value("overrides_cell_colors",false).toBool();
    if(m_override_cell_colors){
        set_color(0,s.value("active_color").value<QColor>());
        set_color(1,s.value("pending_color").value<QColor>());
        set_color(2,s.value("disabled_color").value<QColor>());
    }
}

void CellColors::inherit_colors(const CellColors &cc){
    int idx = 0;
    foreach(CellColorDef *c, cc.m_colors){
        if(!m_override_cell_colors || !c->is_overridden()){
            set_color(idx,cc.get_color(idx));
        }
        idx++;
    }
}

void CellColors::use_defaults(){
    qDeleteAll(m_colors);
    m_colors.clear();
    m_colors.append(new CellColorDef(DT->get_global_color(GCOL_ACTIVE),"active_color",tr("Active"),tr("Color when the related action is enabled and active."),this));
    m_colors.append(new CellColorDef(DT->get_global_color(GCOL_PENDING),"pending_color",tr("Pending"),tr("Color when an action has been flagged to be enabled, but hasn't been yet."),this));
    m_colors.append(new CellColorDef(DT->get_global_color(GCOL_DISABLED),"disabled_color",tr("Disabled"),tr("Color of the cell when the action cannot be toggled."),this));
}

QColor CellColors::get_default_color(int idx) const{
    return DT->get_global_color(static_cast<GLOBAL_COLOR_TYPES>(idx));
}

void CellColors::write_to_ini(QSettings &s){
    //only write values if they've overridden the defaults
    if(m_override_cell_colors){
        s.setValue("overrides_cell_colors",m_override_cell_colors);
        foreach(CellColorDef *c, m_colors){
            s.setValue(c->key(),c->color());
        }
    }
}

QColor CellColors::get_color(int idx) const {
    return m_colors.at(idx)->color();
}

void CellColors::set_color(int idx, QColor c, bool req_check){
    if(req_check){
        QColor def = get_default_color(idx);
        if(c.isValid()){
            m_colors[idx]->set_overridden((c != def));
            m_colors[idx]->set_color(c);
        }else{
            m_colors[idx]->set_overridden(false);
            m_colors[idx]->set_color(def);
        }
    }else{
        m_colors[idx]->set_color(c);
    }
}
