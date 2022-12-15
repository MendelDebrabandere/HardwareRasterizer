#pragma once

namespace dae
{
	class Texture
	{
	public:
		~Texture();

		// rule of 5 copypasta
		Texture(const Texture& other) = delete;
		Texture(Texture&& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) = delete;

		static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);

		ID3D11ShaderResourceView* GetSRV() const;
		//ColorRGB Sample(const Vector2& uv) const;

	private:
		Texture(SDL_Surface* pSurface, ID3D11Device* pDevice);

		ID3D11Texture2D* m_pResource{ nullptr };
		ID3D11ShaderResourceView* m_pSRV{ nullptr };
	};
}

