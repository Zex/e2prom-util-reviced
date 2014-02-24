/*
 * e2prom-util-gui.h
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#ifndef __E2PROM_UTIL_GUI_H
#define __E2PROM_UTIL_GUI_H

#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QDialog>
#include <QProcess>
#include <parameter-panel.h>
#include <tmpl-selt-box.h>

QT_BEGIN_NAMESPACE

class e2prom_util_gui : public QDialog
{
	Q_OBJECT

	tmpl_selt_box *selt_box;
	QPushButton *generate_pbtn;
	QPushButton *console_pbtn;
	parameter_panel *para_panel;
public:
	QTextEdit *result_tedit;
	QProcess gen_eeprom;

public:
	e2prom_util_gui(int weight, int height, QWidget *parent = 0);
	void setup_gen_pbtn();
	void setup_console_pbtn();
	void setup_gui();
	bool is_params_secure(QStringList &argv);

signals:
	void switch_panel(int);
	void has_params(QStringList);
	void start_collect_params();
	void param_panel_size_changed();

public slots:
	void do_generate_eeprom(QStringList argv);
	void show_console_output();
	void switch_param_panel();
	void collect_params();
	void update_result_err();
	void update_result_out();
	void gen_exit(int exit_code, QProcess::ExitStatus exit_stat);
	void gen_error(QProcess::ProcessError exit_stat);
	void adjust_console_tedit();

}; /* class e2prom_util_gui */

QT_END_NAMESPACE

#endif /* __E2PROM_UTIL_GUI_H */
