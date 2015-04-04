
#include <gtkmm.h>
#include <libglademm.h>
#include <bakery/Configuration/Client.h>
#include <libgnomevfsmm.h>
#include <list>

class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  virtual ~MainWindow();

protected:

  virtual Glib::ustring get_path();
  virtual bool build_list_of_files();
  virtual bool build_list_of_files(Glib::ustring& directorypath_uri); //Called recursively.
  virtual Glib::ustring get_new_filepath(const Glib::ustring& filepath);
  static bool file_is_hidden(const Glib::ustring& filepath);
  static void canonical_folder_path(Glib::ustring& folderpath);
  static void get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename);

  // Signal Handlers:
  virtual void on_radio_prefix_clicked();
  virtual void on_radio_suffix_clicked();

  virtual void on_button_choose();
  virtual void on_button_process();
  virtual void on_button_close();

  virtual void on_hide(); //override.

  virtual bool on_transfer_progress(const Gnome::Vfs::Transfer::ProgressInfo& info);

  Glib::RefPtr<Gtk::Builder> m_refGlade;

  // Widgets

  //From Glade file:
  Gtk::Widget* m_pWidgetTopLevel;
  Gtk::RadioButton* m_pRadioPrefix;
  Gtk::RadioButton* m_pRadioSuffix;
  Gtk::Entry* m_pEntryPrefixReplace;
  Gtk::Entry* m_pEntryPrefixWith;
  Gtk::Entry* m_pEntrySuffixReplace;
  Gtk::Entry* m_pEntrySuffixWith;
  Gtk::Entry* m_pEntryPath;
  Gtk::Widget* m_pContainerPrefix;
  Gtk::Widget* m_pContainerSuffix;
  Gtk::Button* m_pButtonChoose;
  Gtk::Button* m_pButtonProcess;
  Gtk::Button* m_pButtonClose;
  Gtk::CheckButton* m_pCheckHidden;
  Gtk::CheckButton* m_pCheckFolders;
  Gtk::CheckButton* m_pCheckRecurse;
  Gtk::ProgressBar* m_pProgressBar;

  //Not from Glade file:
  //Gnome::UI::AppBar m_Status;

  //GConf:
  Bakery::Conf::Client m_ConfClient;

  //List of files to rename:
  typedef std::list<Glib::ustring> type_listStrings;
  type_listStrings m_listFiles, m_listFolders;

  type_listStrings::size_type m_progress_max, m_progress_count;
};

