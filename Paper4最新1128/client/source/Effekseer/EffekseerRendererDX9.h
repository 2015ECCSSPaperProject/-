
#ifndef	__EFFEKSEERRENDERER_DX9_BASE_PRE_H__
#define	__EFFEKSEERRENDERER_DX9_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
//#include <Effekseer.h>
#include "Effekseer.h"

#include <windows.h>
#include <d3d9.h>

#if _WIN32
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "d3d9.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX9_PRE_BASE_H__

#ifndef	__EFFEKSEERRENDERER_RENDERER_H__
#define	__EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
//#include <Effekseer.h>
#include "Effekseer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

/**
	@brief	背景を歪ませるエフェクトを描画する前に実行されるコールバック
*/
class DistortingCallback
{
public:
	DistortingCallback() {}
	virtual ~DistortingCallback() {}

	virtual void OnDistorting() {}
};
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	デバイスロストが発生した時に実行する。
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	デバイスがリセットされた時に実行する。
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destory() = 0;

	/**
		@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	virtual void SetRestorationOfStatesFlag(bool flag) = 0;

	/**
		@brief	描画を開始する時に実行する。
	*/
	virtual bool BeginRendering() = 0;

	/**
		@brief	描画を終了する時に実行する。
	*/
	virtual bool EndRendering() = 0;

	/**
		@brief	ライトの方向を取得する。
	*/
	virtual const ::Effekseer::Vector3D& GetLightDirection() const = 0;

	/**
		@brief	ライトの方向を設定する。
	*/
	virtual void SetLightDirection( ::Effekseer::Vector3D& direction ) = 0;

	/**
		@brief	ライトの色を取得する。
	*/
	virtual const ::Effekseer::Color& GetLightColor() const = 0;

	/**
		@brief	ライトの色を設定する。
	*/
	virtual void SetLightColor( ::Effekseer::Color& color ) = 0;

	/**
		@brief	ライトの環境光の色を取得する。
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const = 0;

	/**
		@brief	ライトの環境光の色を設定する。
	*/
	virtual void SetLightAmbientColor( ::Effekseer::Color& color ) = 0;

		/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	投影行列を取得する。
	*/
	virtual const ::Effekseer::Matrix44& GetProjectionMatrix() const = 0;

	/**
		@brief	投影行列を設定する。
	*/
	virtual void SetProjectionMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	カメラ行列を取得する。
	*/
	virtual const ::Effekseer::Matrix44& GetCameraMatrix() const = 0;

	/**
		@brief	カメラ行列を設定する。
	*/
	virtual void SetCameraMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	カメラプロジェクション行列を取得する。
	*/
	virtual ::Effekseer::Matrix44& GetCameraProjectionMatrix() = 0;

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	virtual ::Effekseer::SpriteRenderer* CreateSpriteRenderer() = 0;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	virtual ::Effekseer::RibbonRenderer* CreateRibbonRenderer() = 0;

	/**
		@brief	リングレンダラーを生成する。
	*/
	virtual ::Effekseer::RingRenderer* CreateRingRenderer() = 0;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	virtual ::Effekseer::ModelRenderer* CreateModelRenderer() = 0;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	virtual ::Effekseer::TrackRenderer* CreateTrackRenderer() = 0;

	/**
		@brief	標準のテクスチャ読込クラスを生成する。
	*/
	virtual ::Effekseer::TextureLoader* CreateTextureLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
		@brief	標準のモデル読込クラスを生成する。
	*/
	virtual ::Effekseer::ModelLoader* CreateModelLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
		@brief	レンダーステートを強制的にリセットする。
	*/
	virtual void ResetRenderState() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを取得する。
	*/
	virtual DistortingCallback* GetDistortingCallback() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを設定する。
	*/
	virtual void SetDistortingCallback(DistortingCallback* callback) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERER_H__
#ifndef	__EFFEKSEERRENDERER_DX9_RENDERER_H__
#define	__EFFEKSEERRENDERER_DX9_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	描画クラス
*/
class Renderer
	: public ::EffekseerRenderer::Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	インスタンスを生成する。
		@param	device	[in]	DirectXのデバイス
		@param	squareMaxCount	[in]	最大描画スプライト数
		@return	インスタンス
	*/
	static Renderer* Create( LPDIRECT3DDEVICE9 device, int32_t squareMaxCount );

	/**
		@brief	デバイスを取得する。
	*/
	virtual LPDIRECT3DDEVICE9 GetDevice() = 0;

	/**
		@brief	デバイスロストリセット間でデバイス自体を再構築する際に外部からデバイスを設定する。
	*/
	virtual void ChangeDevice( LPDIRECT3DDEVICE9 device ) = 0;

	/**
	@brief	背景を取得する。
	*/
	virtual IDirect3DTexture9* GetBackground() = 0;

	/**
	@brief	背景を設定する。
	*/
	virtual void SetBackground(IDirect3DTexture9* background) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	モデル
*/
class Model : public Effekseer::Model
{
private:

public:
	IDirect3DVertexBuffer9*		VertexBuffer;
	IDirect3DIndexBuffer9*		IndexBuffer;
	int32_t						VertexCount;
	int32_t						IndexCount;
	int32_t						FaceCount;
	int32_t						ModelCount;

	Model( uint8_t* data, int32_t size )
		: Effekseer::Model	( data, size )
		, VertexBuffer	( NULL )
		, IndexBuffer	( NULL )
		, VertexCount		( 0 )
		, IndexCount		( 0 )
		, FaceCount			( 0 )
		, ModelCount		( 0 )
	{
	}

	virtual ~Model()
	{
		ES_SAFE_RELEASE( VertexBuffer );
		ES_SAFE_RELEASE( IndexBuffer );
	}
};


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX9_RENDERER_H__