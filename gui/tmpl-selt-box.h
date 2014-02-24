/*
 * tmpl-selt-box.h
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#ifndef __TMPL_SELT_BOX_H
#define __TMPL_SELT_BOX_H

#include <QtGui/QLabel>
#include <QComboBox>
#include <QtGui/QWidget>
#include <QApplication>

QT_BEGIN_NAMESPACE

class tmpl_selt_box
{
	QLabel *select_template_label;
protected:
	void setup_gui(QWidget *parent);

public:
	QComboBox *select_template_cmbox;
	tmpl_selt_box(QWidget *parent);

};

QT_END_NAMESPACE

#endif /* __TMPL_SELT_BOX_H */
