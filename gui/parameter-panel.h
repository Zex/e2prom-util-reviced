/*
 * parameter_panel.h
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#ifndef __PARAMETER_PANEL_H
#define __PARAMETER_PANEL_H

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QApplication>

QT_BEGIN_NAMESPACE

class parameter_panel// : public QObject//_general
{

	QLabel *pi_label;
	QLabel *sn_label;
	QLabel *pd_label;
	QLabel *ti_label;

public:
	QLineEdit *pi_ledit;
	QLineEdit *sn_ledit;
	QLineEdit *pd_ledit;
	QLineEdit *ti_ledit;

	QList<QLabel*> param_names;
	QList<QLineEdit*> param_values;
	
	int x_cur, y_cur, y_pri;
	int w_cur, h_cur;

public:
	void show_panel_fanu();
	void show_panel_abu();
	void show_panel_bscu();
	void show_panel_bcb();
	void show_panel_dibr5();

	QStringList collect_params_fanu();
	QStringList collect_params_abu();
	QStringList collect_params_bscu();
	QStringList collect_params_bcb();
	QStringList collect_params_dibr5();

protected:
	void setup_gui(QWidget *parent);
	void init_pri_param_(QWidget *parent);
	void init_ext_param_(QWidget *parent);
	void update_panel_(int start, int show_end, QStringList lb_names);
	QStringList collect_pri_params_(QString sn_tail);
	QStringList collect_ext_params_(int start, int end, QString sn_tail);

//public slots:
//	void update_panel(int comn);

public:
	parameter_panel(QWidget *parent);

}; /* class parameter_panel_general */

//class parameter_panel_fanu : public parameter_panel_general
//{
//public:
//	parameter_panel_fanu(QWidget *parent)
//	: parameter_panel_general(parent)
//	{}
//}; /* class parameter_panel_fanu */
//
//class parameter_panel_dibr5 : public parameter_panel_general
//{
//	QLabel *sn_label_bscu1;
//	QLineEdit *sn_ledit_bscu1;
//	QLabel *pd_label_bscu1;
//	QLineEdit *pd_ledit_bscu1;
//
//	QLabel *sn_label_bscu2;
//	QLineEdit *sn_ledit_bscu2;
//	QLabel *pd_label_bscu2;
//	QLineEdit *pd_ledit_bscu2;
//
//	QLabel *sn_label_chu1;
//	QLineEdit *sn_ledit_chu1;
//	QLabel *pd_label_chu1;
//	QLineEdit *pd_ledit_chu1;
//
//	QLabel *sn_label_chu2;
//	QLineEdit *sn_ledit_chu2;
//	QLabel *pd_label_chu2;
//	QLineEdit *pd_ledit_chu2;
//
//	QLabel *sn_label_chu3;
//	QLineEdit *sn_ledit_chu3;
//	QLabel *pd_label_chu3;
//	QLineEdit *pd_ledit_chu3;
//
//	QLabel *sn_label_chu4;
//	QLineEdit *sn_ledit_chu4;
//	QLabel *pd_label_chu4;
//	QLineEdit *pd_ledit_chu4;
//
//	QLabel *sn_label_fanu;
//	QLineEdit *sn_ledit_fanu;
//	QLabel *pd_label_fanu;
//	QLineEdit *pd_ledit_fanu;
//
//	QLabel *sn_label_albox;
//	QLineEdit *sn_ledit_albox;
//	QLabel *pd_label_albox;
//	QLineEdit *pd_ledit_albox;
//
//protected:
//	void setup_gui(QWidget *parent);
//public:
//	parameter_panel_dibr5(QWidget *parent);
//	QStringList collect_params();
//	void hide()
//	{
//		pd_label_albox->hide();
//		pd_ledit_albox->hide();
//	}
//}; /* class parameter_panel_dibr5 */
//
//class parameter_panel_bcb : public parameter_panel_general
//{
//	QLabel *sn_label_bcb;
//	QLineEdit *sn_ledit_bcb;
//	QLabel *pd_label_bcb;
//	QLineEdit *pd_ledit_bcb;
//
//	QLabel *sn_label_lps;
//	QLineEdit *sn_ledit_lps;
//	QLabel *pd_label_lps;
//	QLineEdit *pd_ledit_lps;
//	
//	QLabel *sn_label_dpa;
//	QLineEdit *sn_ledit_dpa;
//	QLabel *pd_label_dpa;
//	QLineEdit *pd_ledit_dpa;
//
//protected:
//	void setup_gui(QWidget *parent);
//public:
//	parameter_panel_bcb(QWidget *parent);
//}; /* class parameter_panel_bcb */
//
//class parameter_panel_abu : public parameter_panel_general
//{
//	QLabel *sn_label_alboard;
//	QLineEdit *sn_ledit_alboard;
//	QLabel *pd_label_alboard;
//	QLineEdit *pd_ledit_alboard;
//
//protected:
//	void setup_gui(QWidget *parent);
//public:
//	parameter_panel_abu(QWidget *parent);
//}; /* class parameter_panel_abu */
//
//class parameter_panel_bscu : public parameter_panel_general
//{
//	QLabel *sn_label_mb;
//	QLineEdit *sn_ledit_mb;
//	QLabel *pd_label_mb;
//	QLineEdit *pd_ledit_mb;
//
//	QLabel *sn_label_gnss;
//	QLineEdit *sn_ledit_gnss;
//	QLabel *pd_label_gnss;
//	QLineEdit *pd_ledit_gnss;
//
//	QLabel *sn_label_mcb;
//	QLineEdit *sn_ledit_mcb;
//	QLabel *pd_label_mcb;
//	QLineEdit *pd_ledit_mcb;
//
//protected:
//	void setup_gui(QWidget *parent);
//
//public:
//	parameter_panel_bscu(QWidget *parent);
//
//}; /* class parameter_panel_bscu */

QT_END_NAMESPACE

#endif /* __PARAMETER_PANEL_H */
