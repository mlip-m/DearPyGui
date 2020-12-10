#pragma once

#include "core/AppItems/mvTypeBases.h"
#include <iostream>
#include "mvEvents.h"

//-----------------------------------------------------------------------------
// Widget Index
//
//     * mvSelectable
//     * mvButton
//     * mvCheckbox
//     * mvCombo
//     * mvListbox
//     * mvRadioButton
//     * mvProgressBar
//
//-----------------------------------------------------------------------------

namespace Marvel {

	//-----------------------------------------------------------------------------
	// mvSelectable
	//-----------------------------------------------------------------------------
	class mvSelectable : public mvBoolPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Selectable, "add_selectable")

		mvSelectable(const std::string& name, bool default_value, const std::string& dataSource);

		void setEnabled        (bool value)     override;
		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:
		ImGuiSelectableFlags m_flags = ImGuiSelectableFlags_None;
	};

	//-----------------------------------------------------------------------------
	// mvButton
	//-----------------------------------------------------------------------------
	class mvButton : public mvAppItem
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Button, "add_button")

		mvButton(const std::string& name);

		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:
		bool     m_small = false;
		bool     m_arrow = false;
		ImGuiDir m_direction = ImGuiDir_Up;

	};

	//-----------------------------------------------------------------------------
	// mvButtonSingleton
	//-----------------------------------------------------------------------------
	class mvButtonSingleton : public mvEventHandler
	{

	public:

		mvButtonSingleton(const mvButtonSingleton&) = delete;

		static std::vector<std::pair<int, mvColor>> Colors() { return get()->iColors(); }

	private:

		mvButtonSingleton()
		{
			mvEventBus::Subscribe(this, SID("SET_GLOBAL_COLOR"));
		};

		static mvButtonSingleton* s_instance;

		static mvButtonSingleton* get()
		{
			if (s_instance == nullptr)
				s_instance = new mvButtonSingleton();
			return s_instance;
		}

		bool onEvent(mvEvent& event) override {
			mvEventDispatcher dispatcher(event);

			dispatcher.dispatch(BIND_EVENT_METH(mvButtonSingleton::add_color), SID("SET_GLOBAL_COLOR"));

			return event.handled;
		};

		std::vector<std::pair<int, mvColor>> iColors() { return m_colors; }

		std::vector<std::pair<int, mvColor>> m_colors;

		bool add_color(mvEvent& event)
		{
			if (GetEString(event, "COLOR_CONSTANT") == "mvGuiCol_Button")
			{
				this->m_colors.push_back({ std::make_pair(21, GetEColor(event, "COLOR")) });
			}
			else if (GetEString(event, "COLOR_CONSTANT") == "mvGuiCol_ButtonHovered")
			{
				this->m_colors.push_back({ std::make_pair(22, GetEColor(event, "COLOR")) });
			}
			else if (GetEString(event, "COLOR_CONSTANT") == "mvGuiCol_ButtonActive")
			{
				this->m_colors.push_back({ std::make_pair(23, GetEColor(event, "COLOR")) });
			}
			return false;
		}

	};

	//-----------------------------------------------------------------------------
	// mvCheckbox
	//-----------------------------------------------------------------------------
	class mvCheckbox : public mvBoolPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Checkbox, "add_checkbox")

		mvCheckbox(const std::string& name, bool default_value, const std::string& dataSource);

		void draw() override;

	};

	//-----------------------------------------------------------------------------
	// mvCombo
	//-----------------------------------------------------------------------------
	class mvCombo : public mvStringPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Combo, "add_combo")

		mvCombo(const std::string& name, const std::string& default_value, const std::string& dataSource);

		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:
		ImGuiComboFlags m_flags = ImGuiComboFlags_None;
		std::vector<std::string> m_items;
	};

	//-----------------------------------------------------------------------------
	// mvListbox
	//-----------------------------------------------------------------------------
	class mvListbox : public mvIntPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::Listbox, "add_listbox")

		mvListbox(const std::string& name, int default_value, const std::string& dataSource);

		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:

		std::vector<std::string> m_names;
		int                      m_itemsHeight = 3; // number of items to show (default -1)
		std::vector<const char*> m_charNames;

	};

	//-----------------------------------------------------------------------------
	// mvRadioButton
	//-----------------------------------------------------------------------------
	class mvRadioButton : public mvIntPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::RadioButtons, "add_radio_button")

		mvRadioButton(const std::string& name, int default_value, const std::string& dataSource);

		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:

		std::vector<std::string> m_itemnames;
		bool                     m_horizontal = false;

	};

	//-----------------------------------------------------------------------------
	// mvProgressBar
	//-----------------------------------------------------------------------------
	class mvProgressBar : public mvFloatPtrBase
	{

	public:

		MV_APPITEM_TYPE(mvAppItemType::ProgressBar, "add_progress_bar")

		mvProgressBar(const std::string& name, float default_value, const std::string& dataSource);

		void draw              ()               override;
		void setExtraConfigDict(PyObject* dict) override;
		void getExtraConfigDict(PyObject* dict) override;

	private:

		std::string m_overlay;

	};

}
