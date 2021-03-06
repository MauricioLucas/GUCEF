/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#include "MyGUI_OpenGLRenderManager.h"
#include "MyGUI_OpenGLTexture.h"
#include "MyGUI_OpenGLVertexBuffer.h"
#include "MyGUI_OpenGLDiagnostic.h"
#include "MyGUI_VertexData.h"
#include "MyGUI_Gui.h"
#include "MyGUI_Timer.h"

#include <GLES/gl.h>

namespace MyGUI
{

	OpenGLRenderManager::OpenGLRenderManager() :
		mIsInitialise(false),
		mUpdate(false),
		mImageLoader(nullptr)
	{
	}

	void OpenGLRenderManager::initialise(OpenGLImageLoader* _loader)
	{
		MYGUI_ASSERT(!mIsInitialise, getClassTypeName() << " initialised twice");
		MYGUI_LOG(Info, "* Initialise: " << getClassTypeName());

		mVertexFormat = VertexColourType::ColourABGR;

		mUpdate = false;
		mImageLoader = _loader;

		MYGUI_LOG(Info, getClassTypeName() << " successfully initialized");
		mIsInitialise = true;
	}

	void OpenGLRenderManager::shutdown()
	{
		MYGUI_ASSERT(mIsInitialise, getClassTypeName() << " is not initialised");
		MYGUI_LOG(Info, "* Shutdown: " << getClassTypeName());

		destroyAllResources();

		MYGUI_LOG(Info, getClassTypeName() << " successfully shutdown");
		mIsInitialise = false;
	}

	IVertexBuffer* OpenGLRenderManager::createVertexBuffer()
	{
		return new OpenGLVertexBuffer();
	}

	void OpenGLRenderManager::destroyVertexBuffer(IVertexBuffer* _buffer)
	{
		delete _buffer;
	}

	void OpenGLRenderManager::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count)
	{
		OpenGLVertexBuffer* buffer = static_cast<OpenGLVertexBuffer*>(_buffer);
		unsigned int buffer_id = buffer->getBufferID();
		MYGUI_PLATFORM_ASSERT(buffer_id, "Vertex buffer is not created");

		unsigned int texture_id = 0;
		if (_texture)
		{
			OpenGLTexture* texture = static_cast<OpenGLTexture*>(_texture);
			texture_id = texture->getTextureID();
			//MYGUI_PLATFORM_ASSERT(texture_id, "Texture is not created");
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

		// enable vertex arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// before draw, specify vertex and index arrays with their offsets
		size_t offset = 0;
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offset);
		offset += (sizeof(float) * 3);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offset);
		offset += (4);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offset);

		glDrawArrays(GL_TRIANGLES, 0, _count);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRenderManager::begin()
	{
		//save current attributes
		//glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);  // Unsupported by OpenGL ES
		//glPushAttrib(GL_ALL_ATTRIB_BITS); // Unsupported by OpenGL ES

		//glPolygonMode(GL_FRONT, GL_FILL);  // Unsupported by OpenGL ES
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrthof(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		//glDisable(GL_TEXTURE_GEN_S);  // Unsupported by OpenGL ES
		//glDisable(GL_TEXTURE_GEN_T);  // Unsupported by OpenGL ES
		//glDisable(GL_TEXTURE_GEN_R);  // Unsupported by OpenGL ES

		//glFrontFace(GL_CW);
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
	}

	void OpenGLRenderManager::end()
	{
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		//restore former attributes
		//glPopAttrib();
		//glPopClientAttrib();
	}

	const RenderTargetInfo& OpenGLRenderManager::getInfo()
	{
		return mInfo;
	}

	const IntSize& OpenGLRenderManager::getViewSize() const
	{
		return mViewSize;
	}

	VertexColourType OpenGLRenderManager::getVertexFormat()
	{
		return mVertexFormat;
	}

	void OpenGLRenderManager::drawOneFrame()
	{
		Gui* gui = Gui::getInstancePtr();
		if (gui == nullptr)
			return;

		static Timer timer;
		static unsigned long last_time = timer.getMilliseconds();
		unsigned long now_time = timer.getMilliseconds();
		unsigned long time = now_time - last_time;

		gui->_injectFrameEntered((float)((double)(time) / (double)1000));

		last_time = now_time;

		begin();
		LayerManager::getInstance().renderToTarget(this, mUpdate);
		end();

		mUpdate = false;
	}

	void OpenGLRenderManager::setViewSize(int _width, int _height)
	{
		if (_height == 0)
			_height = 1;
		if (_width == 0)
			_width = 1;

		mViewSize.set(_width, _height);

		mInfo.maximumDepth = 1;
		mInfo.hOffset = 0;
		mInfo.vOffset = 0;
		mInfo.aspectCoef = float(mViewSize.height) / float(mViewSize.width);
		mInfo.pixScaleX = 1.0f / float(mViewSize.width);
		mInfo.pixScaleY = 1.0f / float(mViewSize.height);

		Gui* gui = Gui::getInstancePtr();
		if (gui != nullptr)
		{
			gui->_resizeWindow(mViewSize);
			mUpdate = true;
		}
	}

	ITexture* OpenGLRenderManager::createTexture(const std::string& _name)
	{
		MapTexture::const_iterator item = mTextures.find(_name);
		MYGUI_PLATFORM_ASSERT(item == mTextures.end(), "Texture '" << _name << "' already exist");

		OpenGLTexture* texture = new OpenGLTexture(_name, mImageLoader);
		mTextures[_name] = texture;
		return texture;
	}

	void OpenGLRenderManager::destroyTexture(ITexture* _texture)
	{
		if (_texture == nullptr) return;

		MapTexture::iterator item = mTextures.find(_texture->getName());
		MYGUI_PLATFORM_ASSERT(item != mTextures.end(), "Texture '" << _texture->getName() << "' not found");

		mTextures.erase(item);
		delete _texture;
	}

	ITexture* OpenGLRenderManager::getTexture(const std::string& _name)
	{
		MapTexture::const_iterator item = mTextures.find(_name);
		if (item == mTextures.end()) return nullptr;
		return item->second;
	}

	void OpenGLRenderManager::destroyAllResources()
	{
		for (MapTexture::const_iterator item = mTextures.begin(); item != mTextures.end(); ++item)
		{
			delete item->second;
		}
		mTextures.clear();
	}

} // namespace MyGUI
