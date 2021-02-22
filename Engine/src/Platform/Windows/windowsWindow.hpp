#pragma once

#include "Engine/window.hpp"

#include "Engine/Rendering/GraphicsContext.hpp"

struct GLFWwindow;

namespace Engine
{
	class windowsWindow : public window
	{
	public:
		windowsWindow(const windowProps& props);
		virtual ~windowsWindow();

		void onUpdate() override;

		inline unsigned int getHeight() const override { return m_data.height; }
		inline unsigned int getWidth() const override { return m_data.width; }

		//window attributes:
		inline void setEventCallbackFn(const EventCallbackFn& callback) override { m_data.callback = callback; }
		virtual void setVsync(const bool enabled) override;
		virtual bool isVsync() const override;
		virtual void setDissableCursor(const bool enabled) override;
		virtual bool isDissableCursor() const override;
		virtual void setFullscreen(const bool enabled) override;
		virtual bool isFullscreen() const override;
		virtual void setRawMouseInput(const bool enalbed) override;
		virtual bool isRawMouseInput() const override;

		inline virtual void* getNativeWindow() const { return m_window; }
	private:
		virtual void init(const windowProps& props);
		virtual void shutdown();

		GLFWwindow* m_window;
		graphicsContext* m_context;

		struct windowData
		{
			std::string title;
			unsigned int height, width;
			bool Vsync;
			bool dissableCursor;
			bool fullscreen;
			bool rawMouseInput;

			EventCallbackFn callback;
		};

		windowData m_data;
	};
}