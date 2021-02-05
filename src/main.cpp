// In The Name Of God

#include <bits/stdc++.h>
// #include "SDL2/SDL.h"
// #include "SDL2/SDL_image.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
using namespace std;

// Definition of Classes
class ColliderComponent ;
class AssetManager ;
class Manager ;
class Vector2D ;
//class Collision ;

class Game
{
public:
  Game();
  ~Game();
  void init(const char* title,int x,int y,int width,int height,bool fullscreen);
  void handleevents();
  void update();
  void render();
  void clean();
  bool running();
  static SDL_Renderer*  renderer;
  static SDL_Event event ;
  static bool isrunning ;
  static SDL_Rect camera ;
  static AssetManager* assets ;
private:
  int cnt=0 , val=1 ;
  SDL_Window *window;
};

class AssetManager
{
public:
    AssetManager(Manager* man) ;
    ~AssetManager();

    // Game Objects

    void CreateProjectile(Vector2D pos , Vector2D velocity ,int range,int speed,string id);

    // Texture Management
    void AddTexture(string id , const char* path) ;
    SDL_Texture* GetTexture(string id) ;

private:
  map<string,SDL_Texture*> textures;
  Manager* manager ;
};

class TextureManager{
public:
  static SDL_Texture* LoadTexture(const char *fileName);
  static void Draw(SDL_Texture* tex,SDL_Rect src,SDL_Rect dest,SDL_RendererFlip flip) ;
private:
};
class Mapp
{
public:
  Mapp(string tID,int ms,int ts) ;
  ~Mapp() ;

  void LoadMap(string path,int sizeX,int sizeY) ;
  void AddTile(int srcX,int srcY,int xpos,int ypos);

private:
  string texID ;
  int mapscale ;
  int tilesize ;
  int scaledsize ;
};

class Component ;
class Entity ;
class Vector2D ;
class Manager ;
using ComponentID = size_t ;
using Group = size_t ;
inline ComponentID getNewComponentTypeID()
{
    static ComponentID lastID = 0 ;
    return lastID++;
}

template <typename T>inline ComponentID getComponentTypeID() noexcept
{
  static ComponentID typeID = getNewComponentTypeID() ;
  return typeID;
}

constexpr size_t maxComponents = 32 ;
constexpr size_t maxGroups = 32 ;

using ComponentBitset = bitset<maxComponents>;
using ComponentArray = array<Component*,maxComponents> ;
using GroupBitset = bitset<maxGroups> ;

class Component
{
public:
  Entity* entity ;

  virtual void init(){}
  virtual void update(){}
  virtual void draw() {}

  virtual ~Component(){}
};

class Manager ;
class Entity
{
private:
  Manager& manager ;
  bool active = true ;
  vector<unique_ptr<Component> > components ;

  ComponentArray componentArray ;
  ComponentBitset componentBitset ;
  GroupBitset groupBitset ;

public:
  Entity(Manager& mManager) : manager(mManager) {}

  void update()
  {
    for(auto& c : components) c->update() ;
  }
  void draw(){
    for(auto& c : components) c->draw() ;
  }
  bool isActive() const { return active ; }
  void destroy() { active = false ; }

  bool hasGroups(Group mGroup)
  {
    return groupBitset[mGroup] ;
  }

  void addGroup(Group mGroup) ;
  void delGroup(Group mGroup)
  {
    groupBitset[mGroup] = false;
  }

  template < typename T> bool hasComponents() const
  {
    return componentBitset[getComponentTypeID<T>()];
  }

  template < typename T , typename...TArgs>
  T& addComponent(TArgs&&...mArgs)
  {
    T* c(new T(forward<TArgs>(mArgs)...));
    c->entity = this ;
    unique_ptr<Component> uPtr{c};
    components.emplace_back(move(uPtr)) ;

    componentArray[getComponentTypeID<T>()] = c ;
    componentBitset[getComponentTypeID<T>()] = true ;

    c->init() ;
    return *c;
  }

  template<typename T> T& getComponent() const{
    auto ptr(componentArray[getComponentTypeID<T>()]) ;
    return *static_cast<T*>(ptr);
  }
};
class Vector2D
{
public:
  float x;
  float y;

  Vector2D(){x=y=0.0f ;}
  Vector2D(float x,float y){
    this->x = x ;
    this->y = y ;
  }

  Vector2D& Add(const Vector2D& vec)
  {
    this->x+=vec.x ;
    this->y+=vec.y ;
    return *this ;
  }
  Vector2D& Subtract(const Vector2D& vec)
  {
    this->x-=vec.x ;
    this->y-=vec.y ;
    return *this ;
  }
  Vector2D& Multiply(const Vector2D& vec)
  {
    this->x*=vec.x ;
    this->y*=vec.y ;
    return *this ;
  }
  Vector2D& Divide(const Vector2D& vec)
  {
    this->x/=vec.x ;
    this->y/=vec.y ;
    return *this ;
  }

  friend Vector2D& operator+(Vector2D& v1,const Vector2D& v2){ return v1.Add(v2) ; }
  friend Vector2D& operator-(Vector2D& v1,const Vector2D& v2){ return v1.Subtract(v2) ; }
  friend Vector2D& operator*(Vector2D& v1,const Vector2D& v2){ return v1.Multiply(v2) ; }
  friend Vector2D& operator/(Vector2D& v1,const Vector2D& v2){ return v1.Divide(v2) ; }

  Vector2D& operator+=(const Vector2D& vec) { return this->Add(vec) ; }
  Vector2D& operator-=(const Vector2D& vec) { return this->Subtract(vec) ; }
  Vector2D& operator*=(const Vector2D& vec) { return this->Multiply(vec) ; }
  Vector2D& operator/=(const Vector2D& vec) { return this->Divide(vec) ; }

  Vector2D& operator*(const int& i)
  {
    this->x *= i ;
    this->y *= i ;
    return *this ;
  }
  Vector2D& Zero()
  {
    this->x = 0 ;
    this->y = 0 ;
    return *this ;
  }
//  Vector2D& operator*(const int& i) {}

  friend ostream& operator<<(ostream& stream,const Vector2D vec)
  {
    stream << "( " << vec.x <<"," << vec.y <<" ) ";
    return stream ;
  }
};

class Manager {
private:
  vector<unique_ptr<Entity> > entities ;
  array<vector<Entity*>,maxGroups> groupedEntities ;
public:
  void update()
  {
    for(auto& e : entities) e->update() ;
  }
  void draw()
  {
    for(auto&e : entities) e->draw() ;
  }

  void refresh()
  {
    for(int i=0 ; i<maxGroups ; i++)
    {
        auto& v(groupedEntities[i]) ;
        v.erase(
          remove_if(begin(v),end(v),
            [i](Entity* mEntity)
            {
              return !mEntity->isActive () || !mEntity->hasGroups(i) ;
            }
          ),
          end(v) ) ;
    }

    entities.erase(remove_if(begin(entities),end(entities),
      [](const unique_ptr<Entity> &mEntity)
      {
        return !mEntity->isActive() ;
      }),
        end(entities)) ;
  }

  void AddToGroup(Entity* mEntity , Group mGroup)
  {
    groupedEntities[mGroup].emplace_back(mEntity) ;
  }

  vector<Entity*>& getGroup(Group mGroup)
  {
      return groupedEntities[mGroup] ;
  }

  Entity& addEntity()
  {
    Entity* e = new Entity(*this);
    unique_ptr<Entity> uPtr{e} ;
    entities.emplace_back(move(uPtr)) ;
    return *e;
  }
};

class ColliderComponent ;

class TransformComponent : public Component
{
public:
  Vector2D position ;
  Vector2D velocity ;
  int height = 32 ;
  int width = 32 ;
  int scale = 1 ;
  int speed = 3 ;

  void init() override
  {
     velocity.Zero() ;
  }
  TransformComponent()
  {
    position.Zero() ;
  }
  TransformComponent(float x,float y)
  {
    position.x = x ;
    position.y = y ;
  }
  TransformComponent(int sc)
  {
    // position.Zero() ;
    position.x = 400 ;
    position.y = 320 ;
    scale = sc ;
  }
  TransformComponent(float x,float y,int h,int w,int sc)
  {
    position.x = x ;
    position.y = y ;
    width = w ;
    height = h ;
    scale = sc ;
  }

  void update() override
  {
    position.x += velocity.x * speed ;
    position.y += velocity.y * speed ;
  }

};

struct Animation
{
  int index ;
  int frames ;
  int speed ;
  Animation() {}
  Animation(int i,int f,int s)
  {
    index = i ;
    frames = f ;
    speed = s;
  }
};

class SpriteComponent : public Component
{
private:
  TransformComponent *transform  ;
  SDL_Texture * texture ;
  SDL_Rect srcRect ,destRect ;

  bool animated = false ;
  int frames = 0 ;
  int speed = 100 ;

public:
  int animeIndex = 0 ;
  map<const char*,Animation> animations ;

  SDL_RendererFlip spriteFlip =  SDL_FLIP_NONE ;

  SpriteComponent() = default ;
  SpriteComponent(string id)
  {
    setTex(id) ;
  }
  SpriteComponent(string id,bool isAnimated)
  {
    animated = isAnimated ;
    if(isAnimated)
    {
      Animation idle = Animation(1, 5 , 100) ;
      Animation walked = Animation(0 , 5 , 100 );
      animations.emplace("Idle",idle) ;
      animations.emplace("Walk",walked) ;

      play("Idle") ;
    }
    setTex(id) ;
  }
  ~SpriteComponent()
  {}
  void setTex(string id)
  {
    texture = Game::assets->GetTexture(id) ;
  }
  void init() override
  {
    transform = &entity->getComponent<TransformComponent>() ;

    srcRect.x = srcRect.y = 0;
    srcRect.w = transform->width ;
    srcRect.h = transform->height ;
  }
  void update() override
  {
    if(animated)
    {
      srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed ) % frames) ;
    }
    srcRect.y = animeIndex * transform->height ;

    destRect.x = static_cast<int>(transform->position.x) - Game::camera.x;
    destRect.y = static_cast<int>(transform->position.y) - Game::camera.y;
    destRect.w = transform->width * transform->scale ;
    destRect.h = transform->height * transform->scale ;
  }
  void draw() override
  {
    TextureManager::Draw(texture,srcRect,destRect,spriteFlip) ;
  }
  void play(const char* animeName)
  {
    frames = animations[animeName].frames ;
    speed = animations[animeName].speed ;
    animeIndex = animations[animeName].index ;
  }
};


class KeyboardController : public Component
{
public:
  TransformComponent *transform ;
  SpriteComponent *sprite ;
  void init() override
  {
    transform = &entity->getComponent<TransformComponent>() ;
    sprite = &entity->getComponent<SpriteComponent>() ;
  }
  void update() override
  {
    if(Game::event.type == SDL_KEYDOWN)
    {
      switch (Game::event.key.keysym.sym) {
        case SDLK_w:
          transform->velocity.y = -1 ;
          sprite->play("Walk") ;
          break;
        case SDLK_d:
          transform->velocity.x = 1 ;
          sprite->play("Walk") ;
          break;
        case SDLK_s:
          transform->velocity.y = 1 ;
          sprite->play("Walk") ;
          break;
        case SDLK_a:
          transform->velocity.x = -1 ;
          sprite->play("Walk") ;
          sprite->spriteFlip = SDL_FLIP_HORIZONTAL ;
          break;
        default:
          break;
      }
    }

    if(Game::event.type == SDL_KEYUP)
    {
      switch (Game::event.key.keysym.sym) {
        case SDLK_w:
          transform->velocity.y = 0 ;
          sprite->play("Idle") ;
          break;
        case SDLK_d:
          transform->velocity.x = 0 ;
          sprite->play("Idle") ;
          break;
        case SDLK_s:
          transform->velocity.y = 0 ;
          sprite->play("Idle") ;
          break;
        case SDLK_a:
          transform->velocity.x = 0  ;
          sprite->play("Idle") ;
          sprite->spriteFlip = SDL_FLIP_NONE ;
          break;
        case SDLK_ESCAPE:
          Game::isrunning = false;
        default:
          break;
      }
    }

  }
};


class ColliderComponent : public Component
{
public:
    SDL_Rect collider ;
    string tag ;
    TransformComponent* transform ;

    SDL_Texture * tex ;
    SDL_Rect srcR , destR ;

    ColliderComponent(string t)
    {
      tag = t ;
    }

    ColliderComponent(string t,int xpos,int ypos,int size)
    {
      tag = t ;
      collider.x = xpos ;
      collider.y = ypos ;
      collider.h = collider.w = size ;
    }
    void init() override
    {
      if(entity->hasComponents<TransformComponent>() == false)
      {
        entity->addComponent<TransformComponent>() ;
      }
      transform = &entity->getComponent<TransformComponent>() ;

      tex = TextureManager::LoadTexture("ass/coltex.png") ;
      srcR = { 0,0,32,32 } ;
      destR = {collider.x , collider.y , collider.w , collider.h } ;


    }

    void update() override
    {
      if(tag != "terrain")
      {
        collider.x = static_cast<int>(transform->position.x) ;
        collider.y = static_cast<int>(transform->position.y) ;
        collider.w = transform->width * transform->scale ;
        collider.h = transform->height * transform->scale ;
      }
      destR.x = collider.x - Game::camera.x ;
      destR.y = collider.y - Game::camera.y ;
    }

    void draw() override
    {
      TextureManager::Draw(tex,srcR,destR,SDL_FLIP_NONE) ;
    }
};


class Collision
{
public:
  static bool AABB(const SDL_Rect& recA , const SDL_Rect& recB)
  {
    return (
        recA.x + recA.w >= recB.x  &&
        recB.x + recB.w >= recA.x &&
        recA.y + recA.h >= recB.y &&
        recB.y + recB.h >= recA.y
      );
  }
  static bool AABB(const ColliderComponent& colA,const ColliderComponent& colB)
  {
    if(AABB(colA.collider,colB.collider))
    {
      cout << colA.tag <<" Hit " <<colB.tag << endl;
      return true ;
    }
    return false;
  }
};

class TileComponent : public Component
{
public:
  SDL_Texture * texture ;
  SDL_Rect srcRect , destRect ;
  Vector2D position ;

  TileComponent() = default ;
  TileComponent(int srcX,int srcY,int xpos,int ypos,int tsize , int tscale , string id)
  {
    texture = Game::assets->GetTexture(id) ;

    position.x = xpos ;
    position.y = ypos ;

    srcRect.x = srcX ;
    srcRect.y = srcY ;
    srcRect.w = srcRect.h = tsize ;

    destRect.x = xpos ;
    destRect.y = ypos ;
    destRect.w = destRect.h = tscale * tsize;
  }
  ~TileComponent()
  {
    SDL_DestroyTexture(texture) ;
  }
  void update() override
  {
    destRect.x = position.x - Game::camera.x ;
    destRect.y = position.y - Game::camera.y ;
  }
  void draw() override
  {
    TextureManager::Draw(texture,srcRect,destRect,SDL_FLIP_NONE) ;
  }
};

class ProjectileComponent : public Component
{
public:
  ProjectileComponent(int rng,int sp,Vector2D vel) : range(rng) , speed(sp) , velocity(vel)
  {}
  ~ProjectileComponent()
  {}
  void init() override
  {
    transform = &entity->getComponent<TransformComponent>() ;
    transform->velocity = velocity ;
  }
  void update() override
  {
    cout << transform->position.x << " "<<transform->position.y << endl;
    distance += speed ;

    if(distance>range)
    {
      cout << " REACH RANGE ! " << endl;
      entity->destroy() ;
    }
    else if(transform->position.x > Game::camera.x+Game::camera.w ||
            transform->position.x < Game::camera.x ||
            transform->position.y > Game::camera.y+Game::camera.h ||
            transform->position.y < Game::camera.y
    )
    {
      cout << " OUT OF CAMERA ! " << endl;
      entity->destroy() ;
    }
  }
private:
  TransformComponent*transform ;
  Vector2D velocity ;

  int range=0 ;
  int speed=0 ;
  int distance=0 ;
};

// ########

Mapp* mp ;
Game* game ;
SDL_Renderer* Game::renderer ;
SDL_Event Game::event ;
Manager manager ;
auto& player(manager.addEntity()) ;
bool Game::isrunning = false ;
SDL_Rect Game::camera = {0,0,800,640} ;
AssetManager* Game::assets = new AssetManager(&manager) ;

enum groupLabels : size_t
{
  groupMap,
  groupPlayers,
  groupEnemies,
  groupColliders,
  groupProjectiles
};
int main( int argc, char* args[] )
{
  const int FPS = 250;
  const int frameDelay = 1000/FPS ;
  int frameStart ;
  int frameTime ;

  game = new Game();

  game->init("First Project ! ",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,640,false) ;

  while(game->running())
  {
    frameStart = SDL_GetTicks() ;

    game->handleevents();
    game->update();
    game->render();

    frameTime = SDL_GetTicks() - frameStart ;

    if(frameDelay > frameTime)
    {
      SDL_Delay(frameDelay-frameTime) ;
    }
  }

  game->clean();

  return 0;
}

// Classes

// *** Game ***

auto& tiles(manager.getGroup(groupMap)) ;
auto& colliders(manager.getGroup(groupColliders)) ;
auto& players(manager.getGroup(groupPlayers)) ;
auto& enemies(manager.getGroup(groupEnemies)) ;
auto& projectiles(manager.getGroup(groupProjectiles)) ;

Game::Game(){};
Game::~Game(){};
void Game::init(const char* title,int x,int y,int width,int height,bool fullscreen)
{
  int flags=0 ;
  if(fullscreen)
    flags=SDL_WINDOW_FULLSCREEN;

  if(SDL_Init(SDL_INIT_EVERYTHING)==0)
  {
    cout << "SubSystem initialized ! " << endl;

    window = SDL_CreateWindow(title,x,y,width,height,fullscreen);
    if(window)
    {
      cout << "Window Created ! " << endl;
    }

    renderer = SDL_CreateRenderer(window,-1,0) ;
    if(renderer)
    {
      SDL_SetRenderDrawColor(renderer,255,255,255,255);
      cout << "Renderer Created ! " << endl;
    }

    isrunning = true ;
  }
  else
  {
    isrunning = false ;
  }

  assets->AddTexture("terrain","ass/terrain.png") ;
  assets->AddTexture("player","ass/demon23.png") ;
  assets->AddTexture("projectile","ass/proj.png") ;


  mp = new Mapp("terrain",4,32) ;
  mp->LoadMap("ass/map.map",25,20) ;

  player.addComponent<TransformComponent>(635,635,50,50,2);
  player.addComponent<SpriteComponent>("player",true );
//  player.addComponent<TransformComponent>(400,320,50,50,2);
  //player.addComponent<SpriteComponent>("ass/qw1.png" );
  player.addComponent<KeyboardController>();
  player.addComponent<ColliderComponent>("player") ;
  player.addGroup(groupPlayers) ;

  assets->CreateProjectile(Vector2D(600,600),Vector2D(2,0),200,2,"projectile") ;
}


void Game::handleevents()
{
  SDL_PollEvent(&event);
  switch (event.type) {
    case SDL_QUIT:
      isrunning = false;
      break;
    default:
      break;
  }
}

void Game::update()
{
  SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider ;
  Vector2D playerPos = player.getComponent<TransformComponent>().position ;

  manager.refresh() ;
  manager.update() ;

  for(auto & c : colliders )
  {
    SDL_Rect cCol = c->getComponent<ColliderComponent>().collider ;
    if(Collision::AABB(playerCol,cCol))
    {
      player.getComponent<TransformComponent>().position = playerPos ;
    }
  }

  for(auto& c : projectiles)
  {
    if(Collision::AABB(player.getComponent<ColliderComponent>().collider,c->getComponent<ColliderComponent>().collider))
    {
      c->destroy() ;
      cout << " HITED ! " << endl;
    }
  }

  camera.x = player.getComponent<TransformComponent>().position.x - 400 ;
  camera.y = player.getComponent<TransformComponent>().position.y - 320 ;

  if(camera.x < 0)
     camera.x = 0;
  if(camera.y < 0)
     camera.y = 0;
  if(camera.x > camera.w)
     camera.x = camera.w;
  if(camera.y > camera.h)
     camera.y = camera.h;

/*
  // moving all tiles
  Vector2D pVel = player.getComponent<TransformComponent>().velocity ;
  int pSpeed = player.getComponent<TransformComponent>().speed ;

  for(auto& t:tiles)
  {
    t->getComponent<TileComponent>().destRect.x += -(pVel.x * pSpeed) ;
    t->getComponent<TileComponent>().destRect.y += -(pVel.y * pSpeed) ;
  }
  */
}

void Game::render()
{
  SDL_RenderClear(renderer);
  for(auto& t : tiles)   t->draw() ;
  for(auto& t : colliders) t->draw() ;
  for(auto& t : enemies) t->draw() ;
  for(auto& t : players) t->draw() ;
  for(auto& t : projectiles) t->draw() ;
  SDL_RenderPresent(renderer);
}
void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  cout << "Game Cleaned ! " << endl;
}
bool Game::running()
{
    return isrunning;
}


// *** TextureManager ***

SDL_Texture* TextureManager::LoadTexture(const char* texture)
{
  SDL_Surface* tmpsurface = IMG_Load(texture) ;
  SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer,tmpsurface) ;
  SDL_FreeSurface(tmpsurface);

  return tex;
}

void TextureManager::Draw(SDL_Texture* tex,SDL_Rect src,SDL_Rect dest,SDL_RendererFlip flip )
{
  SDL_RenderCopyEx(Game::renderer,tex,&src,&dest,NULL,NULL,flip) ;
  // SDL_RenderCopy(Game::renderer,tex,&src,&dest) ;
}

// *** Mapp ***

Mapp::Mapp(string tID,int ms,int ts) :texID(tID) , mapscale(ms) , tilesize(ts)
{
  scaledsize = ms*ts ;
}

Mapp::~Mapp()
{
}

void Mapp::LoadMap(string path,int sizeX,int sizeY)
{
  char c ;
  fstream mapfile ;
  mapfile.open(path) ;

  for(int y=0 ; y<sizeY ; y++)
  {
    for(int x=0 ; x<sizeX ; x++)
    {
      mapfile.get(c) ;
      int srcY = atoi(&c) * tilesize;
      mapfile.get(c) ;
      int srcX = atoi(&c) * tilesize ;
      //cout << x*scaledsize<<" "<<y*scaledsize <<" => "<< srcX <<" "<<srcY << endl;
      AddTile(srcX,srcY,x*scaledsize,y*scaledsize) ;
      mapfile.ignore() ;
    }
  }
//  return ;
  //mapfile.ignore() ;
  for(int y=0 ; y<sizeY ; y++)
  {
    for(int x=0 ; x<sizeX ; x++)
    {
      mapfile.get(c) ;
      //cout << c << endl;
      if(c == '1')
      {
      //  cout << "?" << endl;
        auto& tcol(manager.addEntity()) ;
        tcol.addComponent<ColliderComponent>("terrain",x*scaledsize,y*scaledsize,scaledsize) ;
        tcol.addGroup(groupColliders) ;
      }
      mapfile.ignore() ;
    }
  }

  mapfile.close() ;
}

void Mapp::AddTile(int srcX,int srcY,int xpos,int ypos)
{
  auto& tile(manager.addEntity()) ;
  tile.addComponent<TileComponent>(srcX,srcY,xpos,ypos,tilesize,mapscale,texID) ;
  tile.addGroup(groupMap) ;
}

void Entity::addGroup(Group mGroup)
{
  groupBitset[mGroup] = true ;
  manager.AddToGroup(this,mGroup) ;
}

AssetManager::AssetManager(Manager* man) : manager(man)
{}
AssetManager::~AssetManager()
{}

void AssetManager::AddTexture(string id , const char* path)
{
  textures.emplace(id,TextureManager::LoadTexture(path)) ;
}

SDL_Texture* AssetManager::GetTexture(string id)
{
  return textures[id] ;
}

void AssetManager::CreateProjectile(Vector2D pos , Vector2D velocity,int range,int speed,string id)
{
  auto& projectile(manager->addEntity()) ;
  projectile.addComponent<TransformComponent>(pos.x,pos.y,32,32,1) ;
  projectile.addComponent<SpriteComponent>(id,false) ;
  projectile.addComponent<ProjectileComponent>(range,speed,velocity) ;
  projectile.addComponent<ColliderComponent>("projectile") ;
  projectile.addGroup(groupProjectiles) ;
}
