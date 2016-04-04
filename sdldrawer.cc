
class SDLDrawer
{
public:

	std::shared_ptr<SDL_Window>  window;
	
	struct SDL_Renderer_delete
	{void operator()(SDL_Renderer* x){SDL_DestroyRenderer(x);}};
	std::unique_ptr<SDL_Renderer,SDL_Renderer_delete> renderer;
	
	struct SDL_Texture_delete
	{void operator()(SDL_Texture* x){SDL_DestroyTexture(x);}};
	std::unique_ptr<SDL_Texture,SDL_Texture_delete> texture;
	
	
	int width, height;
	int rw,rh;
	
	
	SDLDrawer(std::shared_ptr<SDL_Window> window_):window(window_)
	{
		SDL_GetWindowSize(window.get(),&width,&height);
		renderer=std::unique_ptr<SDL_Renderer,SDL_Renderer_delete>(SDL_CreateRenderer(window.get(),-1, SDL_RENDERER_TARGETTEXTURE));
		texture=std::unique_ptr<SDL_Texture,SDL_Texture_delete>(SDL_CreateTexture(renderer.get(), 0, SDL_TEXTUREACCESS_TARGET,width, height));
		Clear();
		Present();
	}
	
	SDLDrawer( SDLDrawer& other ) = delete;  //must not allow copy constructor!

	
	void Render()     //renders to the texture
	{
		SDL_SetRenderTarget(renderer.get(), texture.get());
		SDL_RenderPresent(renderer.get());
	}
		
	void Present()    //renders the texture to the screen
	{
		SDL_SetRenderTarget(renderer.get(), NULL);
		SDL_RenderCopy(renderer.get(),texture.get(), NULL, NULL);
		SDL_RenderPresent(renderer.get());
	}
	
	void Clear()
	{
		SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0xFF );
		SDL_SetRenderTarget(renderer.get(), texture.get());
		SDL_RenderClear(renderer.get());
		SDL_RenderPresent(renderer.get());
	}
	
	inline void Colour(int red, int green, int blue)
	{
		SDL_SetRenderDrawColor(renderer.get(), red, green, blue, 0xFF );	
	}
	
	inline void DrawPoint(int x, int y)
	{
		SDL_RenderDrawPoint(renderer.get(),x,y);
	}
	
	
	inline void DrawPoints(const std::vector<std::pair<int,int>> &points)
	{
		SDL_RenderDrawPoints(renderer.get(), (SDL_Point*) points.data(), points.size());
	}
	
	inline void DrawLine(int x, int y, int xx, int yy)
	{
		SDL_RenderDrawLine(renderer.get(),x,y,xx,yy);
	}
	
	inline void DrawRect(int x, int y, int w, int h)
	{
		SDL_Rect rect;
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
		SDL_RenderDrawRect(renderer.get(), &rect);
	}
	
	void DrawCircle(int xx, int yy, int r)
	{
		std::vector<SDL_Point> points;
		
		int x = r;
		int y = 0;
		int re = 1-x;
		
		while(x >= y)
		{
			points.push_back({x + xx, y + yy});
			points.push_back({y + xx, x + yy});
			points.push_back({-x + xx, y + yy});
			points.push_back({-y + xx, x + yy});
			points.push_back({-x + xx, -y + yy});
			points.push_back({-y + xx, -x + yy});
			points.push_back({x + xx, -y + yy});
			points.push_back({y + xx, -x + yy});
			y++;
			if (re<0)
			{
			re += 2 * y + 1;
			}else{
				x--;
				re += 2 * (y - x + 1);
			}
		}
		SDL_RenderDrawPoints(renderer.get(), points.data(), points.size());
	}
	
	
	void DrawSolidCircle(int xx, int yy, int r)
	{
		std::vector<SDL_Rect> rects;
		
		int x = r;
		int y = 0;
		int re = 1-x;
		
		while(x >= y)
		{
			rects.push_back({-x + xx, y + yy,2*x,1});
			rects.push_back({-y + xx, x + yy,2*y,1});
			rects.push_back({-y + xx, -x + yy,2*y,1});
			rects.push_back({-x + xx, -y + yy,2*x,1});
			y++;
			if (re<0)
			{
			re += 2 * y + 1;
			}else{
				x--;
				re += 2 * (y - x + 1);
			}
		}
		SDL_RenderDrawRects(renderer.get(), rects.data(), rects.size());
	}
	
	inline void DrawSolidRect(int x, int y, int w, int h)
	{
		SDL_Rect rect;
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
		SDL_RenderFillRect(renderer.get(), &rect);
	}
	
	void DrawSurface(SDL_Surface *surface)
	{
		SDL_Texture* t = SDL_CreateTextureFromSurface(renderer.get(), surface);
		SDL_RenderCopy(renderer.get(), t, NULL, NULL);
		SDL_DestroyTexture(t);
	}
	
};
