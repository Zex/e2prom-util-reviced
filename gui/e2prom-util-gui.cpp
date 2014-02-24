/*
 * e2prom-util-gui.cpp
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#include <QtGui>
#include <e2prom-util-gui.h>
//#include <QMessageBox>
#include <QDateTime>

QT_BEGIN_NAMESPACE
	
e2prom_util_gui::e2prom_util_gui(int width, int height, QWidget *parent)
: QDialog(parent)
{
	resize(width, height);
	setWindowTitle("E2prom Util");
	setup_gui();
	QApplication::restoreOverrideCursor();

	connect(selt_box->select_template_cmbox, SIGNAL(currentIndexChanged(int)), this, SLOT(switch_param_panel()));
	connect(console_pbtn, SIGNAL(clicked()), this, SLOT(show_console_output()));
	connect(generate_pbtn, SIGNAL(clicked()), this, SLOT(collect_params()));
	connect(this, SIGNAL(has_params(QStringList)), this, SLOT(do_generate_eeprom(QStringList)));
	connect(this, SIGNAL(param_panel_size_changed()), this, SLOT(adjust_console_tedit()));
	connect(console_pbtn, SIGNAL(clicked()), this, SLOT(adjust_console_tedit()));
	connect(&gen_eeprom, SIGNAL(readyReadStandardError()), this, SLOT(update_result_err()));
	connect(&gen_eeprom, SIGNAL(readyReadStandardOutput()), this, SLOT(update_result_out()));
	connect(&gen_eeprom, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(gen_exit(int, QProcess::ExitStatus)));
	connect(&gen_eeprom, SIGNAL(error(QProcess::ProcessError)), this, SLOT(gen_error(QProcess::ProcessError)));
}

void e2prom_util_gui::gen_error(QProcess::ProcessError exit_stat)
{
	if (exit_stat == QProcess::FailedToStart) 
		result_tedit->append(tr("Failed to start temp2bin"));
		
	generate_pbtn->setEnabled(true);
}

void e2prom_util_gui::gen_exit(int exit_code, QProcess::ExitStatus exit_stat)
{
	if (exit_stat == QProcess::CrashExit) {

	result_tedit->append(tr("temp2bin crashed"));

	} else if (exit_code != 0) {

	result_tedit->append(tr("Generation failed"));

	} else {

	result_tedit->append(tr("Generation finished"));//.arg(targetFile));

	}
	generate_pbtn->setEnabled(true);
}

void e2prom_util_gui::update_result_out()
{
    QByteArray out = gen_eeprom.readAllStandardOutput();
//    QString text = result_tedit->toPlainText() + QString::fromLocal8Bit(out);
//    QString text = QDateTime::currentDateTime().toString()+'\n'+QString::fromLocal8Bit(out);
    result_tedit->append(QString::fromLocal8Bit(out));
}

void e2prom_util_gui::update_result_err()
{
    QByteArray err = gen_eeprom.readAllStandardError();
//    QString text = result_tedit->toPlainText() + QString::fromLocal8Bit(err);
//    QString text = QDateTime::currentDateTime().toString()+'\n'+QString::fromLocal8Bit(err);
    result_tedit->append(QString::fromLocal8Bit(err));
}

void e2prom_util_gui::switch_param_panel()
{
//	emit switch_panel(selt_box->select_template_cmbox->currentIndex());
	switch (selt_box->select_template_cmbox->currentIndex()) {
	case 0: para_panel->show_panel_fanu(); break;
	case 1: para_panel->show_panel_abu(); break;
	case 2: para_panel->show_panel_bcb(); break;
	case 3:	para_panel->show_panel_bscu(); break;
	case 4: para_panel->show_panel_dibr5(); break;
	default:;
	}
	emit param_panel_size_changed();
}

void e2prom_util_gui::collect_params()
{
	generate_pbtn->setEnabled(false);

	switch (selt_box->select_template_cmbox->currentIndex()) {
	case 0: emit has_params(para_panel->collect_params_fanu()); break;
	case 1: emit has_params(para_panel->collect_params_abu()); break;
	case 2: emit has_params(para_panel->collect_params_bcb()); break;
	case 3:	emit has_params(para_panel->collect_params_bscu()); break;
	case 4: emit has_params(para_panel->collect_params_dibr5()); break;
	default:;
	}
}

void e2prom_util_gui::adjust_console_tedit()
{
	if (!result_tedit->isVisible()) {
		resize(para_panel->w_cur+10, para_panel->y_cur+10);
	} else {
		result_tedit->setGeometry(QRect(para_panel->x_cur, para_panel->y_cur+10, 791, 281));
		resize(para_panel->x_cur+result_tedit->width()+10, para_panel->y_cur+10+result_tedit->height()+10);
	}
	update();
//	updateGeometry();
}

void e2prom_util_gui::show_console_output()
{
//	result_tedit->isVisible() ? (result_tedit->hide() : result_tedit->show();
	if (result_tedit->isVisible()) {
		result_tedit->hide();
		result_tedit->setGeometry(QRect(para_panel->x_cur, para_panel->y_cur+10, 0, 0));
	} else {
		result_tedit->show();
	}
}
	
bool e2prom_util_gui::is_params_secure(QStringList &argv)
{
#ifdef MinGW
	QString cmd("temp2bin.exe");
#else
	QString cmd("temp2bin");
#endif

	for (int i = 0; i < argv.size(); i++) {
		if (!argv[i].size()) return false;
		cmd += ' ';
		cmd += argv[i];
	}

	result_tedit->append(cmd);//.toStdString().c_str());
	return true;
}

void e2prom_util_gui::do_generate_eeprom(QStringList argv)
{
	QStringList buf;
	QString tmpl_dir("templates");

	buf += tmpl_dir + QDir::separator() + selt_box->select_template_cmbox->currentText() + ".txt";
	buf += argv;
	argv = buf;

    result_tedit->setText(QDateTime::currentDateTime().toString());
	if (is_params_secure(argv)) {
#ifdef MinGW
	QString cmd("temp2bin.exe");
#else
	QString cmd("temp2bin");
#endif
		gen_eeprom.start(cmd, argv);
	} else {
		result_tedit->append(tr("Paramerters are too few to generate eeprom"));
		generate_pbtn->setEnabled(true);
	}
}

void e2prom_util_gui::setup_gen_pbtn()
{
	generate_pbtn = new QPushButton(this);
	generate_pbtn->setGeometry(QRect(690, 20, 111, 31));
	generate_pbtn->setText(QApplication::translate("E2prom Util", "Generate", 0, QApplication::UnicodeUTF8));
}

void e2prom_util_gui::setup_console_pbtn()
{
	console_pbtn = new QPushButton(this);
	console_pbtn->setGeometry(QRect(550, 20, 111, 31));
	console_pbtn->setText(QApplication::translate("E2prom Util", "Console", 0, QApplication::UnicodeUTF8));
}

void e2prom_util_gui::setup_gui()
{
	selt_box = new tmpl_selt_box(this);
	setup_gen_pbtn();
	setup_console_pbtn();
	para_panel = new parameter_panel(this);

	result_tedit = new QTextEdit(this);
	result_tedit->hide();
}

QT_END_NAMESPACE

