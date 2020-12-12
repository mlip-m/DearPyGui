#include <utility>
#include "mvBasicItems.h"
#include "mvApp.h"
#include "Registries/mvValueStorage.h"
#include "PythonUtilities/mvPythonTranslator.h"

namespace Marvel {

	mvSelectable::mvSelectable(const std::string& name, bool default_value, const std::string& dataSource)
		: mvBoolPtrBase(name, default_value, dataSource)
	{
		m_description.disableAllowed = true;
	}

	void mvSelectable::setEnabled(bool value)
	{
		if (value)
			m_flags &= ~ImGuiSelectableFlags_Disabled;

		else
			m_flags |= ImGuiSelectableFlags_Disabled;

		m_enabled = value;
	}

	void mvSelectable::draw()
	{
		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		if (ImGui::Selectable(m_label.c_str(), m_value, m_flags))
		{

			mvCallbackRegistry::GetCallbackRegistry()->addCallback(m_callback, m_name, m_callbackData);

			// Context Menu
			if (!getPopup().empty())
				ImGui::OpenPopup(getPopup().c_str());
		}

	}

	void mvSelectable::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;

		// helper for bit flipping
		auto flagop = [dict](const char* keyword, int flag, int& flags, bool flip)
		{
			if (PyObject* item = PyDict_GetItemString(dict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
		};

		// window flags
		flagop("span_columns", ImGuiSelectableFlags_SpanAllColumns, m_flags, false);

	}

	void mvSelectable::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;

		// helper to check and set bit
		auto checkbitset = [dict](const char* keyword, int flag, const int& flags, bool flip)
		{
			PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
		};

		// window flags
		checkbitset("span_columns", ImGuiSelectableFlags_SpanAllColumns, m_flags, false);
	}

	mvAppItemTheme buttonTheme = mvAppItemTheme{ 1 };

	mvButton::mvButton(const std::string& name)
		: mvAppItem(name)
	{
		m_description.disableAllowed = true;
	}

	void mvButton::draw()
	{
		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		for (auto& item : buttonTheme.getColors())
		{
			ImGui::PushStyleColor(item.first, item.second.toVec4());
		}

		if (!m_enabled)
		{
			ImVec4 disabled_color = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			disabled_color.w = 0.392f;
			styleManager.addColorStyle(ImGuiCol_Button, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ButtonHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ButtonActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_Text, ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
		}

		if (m_small)
		{
			if (ImGui::SmallButton(m_label.c_str()))
				mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);

			for (auto& item : buttonTheme.getColors())
			{
				ImGui::PopStyleColor(1);
			}

			return;
		}

		if (m_arrow)
		{
			if (ImGui::ArrowButton(m_label.c_str(), m_direction))
				mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);

			for (auto& item : buttonTheme.getColors())
			{
				ImGui::PopStyleColor(1);
			}

			return;
		}

		if (ImGui::Button(m_label.c_str(), ImVec2((float)m_width, (float)m_height)))
			mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);

		for (auto& item : buttonTheme.getColors())
		{
			ImGui::PopStyleColor(1);
		}

	}

	void mvButton::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;

		if (PyObject* item = PyDict_GetItemString(dict, "small")) m_small = ToBool(item);
		if (PyObject* item = PyDict_GetItemString(dict, "arrow")) m_arrow = ToBool(item);
		if (PyObject* item = PyDict_GetItemString(dict, "direction")) m_direction = ToInt(item);
	}

	void mvButton::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		PyDict_SetItemString(dict, "small", ToPyBool(m_small));
		PyDict_SetItemString(dict, "arrow", ToPyBool(m_arrow));
		PyDict_SetItemString(dict, "direction", ToPyInt(m_direction));
	}

	mvCheckbox::mvCheckbox(const std::string& name, bool default_value, const std::string& dataSource)
		: mvBoolPtrBase(name, default_value, dataSource)
	{
		m_description.disableAllowed = true;
	}

	void mvCheckbox::draw()
	{
		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		if (!m_enabled)
		{
			ImVec4 disabled_color = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			disabled_color.w = 0.392f;
			styleManager.addColorStyle(ImGuiCol_CheckMark, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBg, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_Text, ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
			m_disabled_value = *m_value;
		}

		if (ImGui::Checkbox(m_label.c_str(), m_enabled ? m_value : &m_disabled_value))
			mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);

	}

	mvCombo::mvCombo(const std::string& name, const std::string& default_value, const std::string& dataSource)
		: mvStringPtrBase(name, default_value, dataSource)
	{
		m_description.disableAllowed = true;
	}

	void mvCombo::draw()
	{

		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		static std::vector<std::string> disabled_items{};
		if (!m_enabled)
		{
			ImVec4 disabled_color = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			disabled_color.w = 0.392f;
			styleManager.addColorStyle(ImGuiCol_FrameBg, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_Button, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ButtonHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ButtonActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_PopupBg, { 0.0f, 0.0f, 0.0f, 0.0f });
			styleManager.addColorStyle(ImGuiCol_Border, { 0.0f, 0.0f, 0.0f, 0.0f });
			styleManager.addColorStyle(ImGuiCol_Text, ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
		}
		if (ImGui::BeginCombo(m_label.c_str(), m_value->c_str(), m_flags)) // The second parameter is the label previewed before opening the combo.
		{
			for (const auto& name : m_enabled ? m_items : disabled_items)
			{
				bool is_selected = (*m_value == name);
				if (ImGui::Selectable((name).c_str(), is_selected))
				{
					if (m_enabled) { *m_value = name; }
					mvCallbackRegistry::GetCallbackRegistry()->addCallback(m_callback, m_name, m_callbackData);

				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}

			ImGui::EndCombo();
		}

	}

	void mvCombo::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		if (PyObject* item = PyDict_GetItemString(dict, "items")) m_items = ToStringVect(item);

		// helpers for bit flipping
		auto flagop = [dict](const char* keyword, int flag, int& flags)
		{
			if (PyObject* item = PyDict_GetItemString(dict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
		};

		auto conflictingflagop = [dict](const std::vector<std::string>& keywords, std::vector<int> flags, int& mflags)
		{

			for (size_t i = 0; i < keywords.size(); i++)
			{
				if (PyObject* item = PyDict_GetItemString(dict, keywords[i].c_str()))
				{
					//turning all conflicting flags false
					for (const auto& flag : flags) mflags &= ~flag;
					//writing only the first conflicting flag
					ToBool(item) ? mflags |= flags[i] : mflags &= ~flags[i];
					break;
				}
			}

		};

		flagop("popup_align_left", ImGuiComboFlags_PopupAlignLeft, m_flags);
		flagop("no_arrow_button", ImGuiComboFlags_NoArrowButton, m_flags);
		flagop("no_preview", ImGuiComboFlags_NoPreview, m_flags);

		std::vector<std::string> HeightKeywords{
			"height_small",
			"height_regular",
			"height_large",
			"height_largest" };
		std::vector<int> HeightFlags{
			ImGuiComboFlags_HeightSmall,
			ImGuiComboFlags_HeightRegular,
			ImGuiComboFlags_HeightLarge,
			ImGuiComboFlags_HeightLargest };

		conflictingflagop(HeightKeywords, HeightFlags, m_flags);

	}

	void mvCombo::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		PyDict_SetItemString(dict, "items", ToPyList(m_items));

		// helper to check and set bit
		auto checkbitset = [dict](const char* keyword, int flag, const int& flags)
		{
			PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
		};
		checkbitset("popup_align_left", ImGuiComboFlags_PopupAlignLeft, m_flags);
		checkbitset("height_small", ImGuiComboFlags_HeightSmall, m_flags);
		checkbitset("height_regular", ImGuiComboFlags_HeightRegular, m_flags);
		checkbitset("height_large", ImGuiComboFlags_HeightLarge, m_flags);
		checkbitset("height_largest", ImGuiComboFlags_HeightLargest, m_flags);
		checkbitset("no_arrow_button", ImGuiComboFlags_NoArrowButton, m_flags);
		checkbitset("no_preview", ImGuiComboFlags_NoPreview, m_flags);
	}

	mvListbox::mvListbox(const std::string& name, int default_value, const std::string& dataSource)
		: mvIntPtrBase(name, default_value, dataSource)
	{
		m_description.disableAllowed = true;
	}

	void mvListbox::draw()
	{
		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		if (!m_enabled)
		{
			ImVec4 disabled_color = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			disabled_color.w = 0.392f;
			styleManager.addColorStyle(ImGuiCol_Header, disabled_color);
			styleManager.addColorStyle(ImGuiCol_HeaderHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_HeaderActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBg, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ScrollbarBg, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ScrollbarGrab, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ScrollbarGrabHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_ScrollbarGrabActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_Text, ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
			m_disabled_value = *m_value;
		}

		if (ImGui::ListBox(m_label.c_str(), m_enabled ? m_value : &m_disabled_value, m_charNames.data(), (int)m_names.size(), m_itemsHeight))
			mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);

	}

	void mvListbox::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		if (PyObject* item = PyDict_GetItemString(dict, "num_items")) m_itemsHeight = ToInt(item);
		if (PyObject* item = PyDict_GetItemString(dict, "items"))
		{
			m_names = ToStringVect(item);
			m_charNames.clear();
			for (const std::string& item : m_names)
				m_charNames.emplace_back(item.c_str());
		}
	}

	void mvListbox::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		PyDict_SetItemString(dict, "items", ToPyList(m_names));
		PyDict_SetItemString(dict, "num_items", ToPyInt(m_itemsHeight));
	}

	mvRadioButton::mvRadioButton(const std::string& name, int default_value, const std::string& dataSource)
		: mvIntPtrBase(name, default_value, dataSource)
	{
		m_description.disableAllowed = true;
	}

	void mvRadioButton::draw()
	{

		ImGui::BeginGroup();

		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		if (!m_enabled)
		{
			ImVec4 disabled_color = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			disabled_color.w = 0.392f;
			styleManager.addColorStyle(ImGuiCol_CheckMark, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBg, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgHovered, disabled_color);
			styleManager.addColorStyle(ImGuiCol_FrameBgActive, disabled_color);
			styleManager.addColorStyle(ImGuiCol_Text, ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
			m_disabled_value = *m_value;
		}
		for (size_t i = 0; i < m_itemnames.size(); i++)
		{
			if (m_horizontal && i != 0)
				ImGui::SameLine();

			if (ImGui::RadioButton((m_itemnames[i] + "##" + m_name).c_str(), m_enabled ? m_value : &m_disabled_value, (int)i))
				mvCallbackRegistry::GetCallbackRegistry()->addCallback(getCallback(false), m_name, m_callbackData);
		}

		ImGui::EndGroup();
	}

	void mvRadioButton::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		if (PyObject* item = PyDict_GetItemString(dict, "items")) m_itemnames = ToStringVect(item);
		if (PyObject* item = PyDict_GetItemString(dict, "horizontal")) m_horizontal = ToBool(item);
	}

	void mvRadioButton::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		PyDict_SetItemString(dict, "items", ToPyList(m_itemnames));
		PyDict_SetItemString(dict, "horizontal", ToPyBool(m_horizontal));
	}

	mvProgressBar::mvProgressBar(const std::string& name, float default_value, const std::string& dataSource)
		: mvFloatPtrBase(name, default_value, dataSource)
	{
	}

	void mvProgressBar::draw()
	{
		auto styleManager = m_styleManager.getScopedStyleManager();
		ScopedID id;

		ImGui::ProgressBar(*m_value, ImVec2((float)m_width, (float)m_height), m_overlay.c_str());

	}

	void mvProgressBar::setExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		if (PyObject* item = PyDict_GetItemString(dict, "overlay")) m_overlay = ToString(item);
	}

	void mvProgressBar::getExtraConfigDict(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		mvGlobalIntepreterLock gil;
		PyDict_SetItemString(dict, "overlay", ToPyString(m_overlay));
	}
}