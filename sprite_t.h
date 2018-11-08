#ifndef SPRITE_T_H_INCLUDED
#define SPRITE_T_H_INCLUDED
#include<bits/stdc++.h>
#include"pointd.h"
#include"SDL2_header.h"


std::map<int, bool> keyboard;
const int default_life=12;
const int Game::SCREEN_WIDTH	= 640;
const int Game::SCREEN_HEIGHT	= 480;
Mix_Chunk* soundbullet,*soundexplosion,*sounddie,*soundhurt,*soundenemyhurt,*soundsuperweapon;
Mix_Music* sound;
const std::string RES_PATH_MUS	= getResourcePath("sound");
Image *imagePlayer[2], *imagebullet_player, *imageEnemy,*imageexplosion,*imagebullet_enemy ;
//detect collide between bullet_player and enemy,player and enemy
/*template<class T1,class T2>

bool collide(T1 a,T2 b)
{
    return (a.pos.x>=b.pos.x-a.crashing_width and a.pos.x<=b.pos.x+b.crashing_width and a.pos.y>=b.pos.y-a.crashing_length and a.pos.y<=b.pos.y+b.crashing_length );
}*/
template<class T1,class T2>
bool collide(T1 a,T2 b)
{
    return (fabs(a.pos.x-b.pos.x)<=(a.crashing_width+b.crashing_width)/2 and fabs(a.pos.y-b.pos.y)<=(a.crashing_length+b.crashing_length)/2);
}
//detect collide between bullet_enemy and player
template<class T1,class T2>
bool collide2(T1 a,T2 b)
{
    return (fabs(a.pos.x-b.pos.x)<=(a.width+b.width)/2 and fabs(a.pos.y-b.pos.y)<=(a.length+b.length)/2);
}
void keyDown()
{
    keyboard[Game::keyValue] = true;
}

void keyUp()
{
    keyboard[Game::keyValue] = false;
}

PointD drawtext(const std::string &str,int  posx,int posy,int w_rate=1,int h_rate=1)
{
    Image* text=Game::textToImage(str);
    int w,h;
    Game::getImageSize(text,w,h);
    Game::drawImage(text,posx-w/2,posy-h);
    cleanup(text);
}
Mix_Chunk* loadmusic(const char* a )
{
    char address[100],b[100];
    std::stringstream ss;
    ss<<RES_PATH_MUS;
    ss>>b;
    ss.clear();
    strcpy(address,b);
    strcat(address,a);
    Mix_Chunk* sound=Mix_LoadWAV(address);
    return sound;
}
void playmusic(Mix_Chunk* music)
{
    Mix_PlayChannel( -1, music, 0 );
}
class sprite_t
{
public:
    int life;
    PointD pos;
    //size used for collide
    int  width;
    int length;
    double speed;
    PointD velocity;
    //size used for collide2
    int crashing_width;
    int crashing_length;
    bool alive()
    {
        return life>0;
    }
    Image* image;
    void draw()
    {
        Game::drawImage( image, pos.x-crashing_width/2,pos.y-crashing_length/2 );
    }
} ;
//the position where the bullet is added
class fire
{
public:
    fire(const PointD &b)
    {
        pos=b;
    }
    PointD pos;
};

class player:public sprite_t
{
public:
    player(const int &a)
    {
        width=2;
        length=2;
        reborn=false;
        pos=PointD(Game::SCREEN_WIDTH/2,Game::SCREEN_HEIGHT/2 );
        shootings.push_back(fire(pos));
        score=0;
        life=5;
        speed = 5;
        superweapons=1;
        cooling_down=0;
        image=imagePlayer[a];
        Game::getImageSize(image,crashing_width,crashing_length);
    }
    std::vector<fire> shootings;
    int score;
    //record whether a player is in supertime status(caused by injury)  and how long the remaining super time
    bool reborn;
    int supertime;
    bool move;
    //the number of superweapons available
    int superweapons;
    //the cooling down time of superweapon
    int cooling_down;
    void setvisible(){
        if(alive() and reborn)
        {
            Game::setImageAlpha(image,128);
        }
        else if(!reborn and alive())
        {
            Game::setImageAlpha(image,255);
        }
        else
        {
            //dead
            Game::setImageAlpha(image,0);
        }
    }


    void update()
    {
        //update reborn status
        if(reborn)
        {
            supertime--;
            if(supertime<0)
            {
                reborn=false;
            }
        }
        if(cooling_down>0)
        {
            cooling_down--;
        }
        //Stop player
        if(!move)
        {
            velocity = velocity * 0.8;
            if(velocity.length() < 0.1 )
                velocity = PointD();
        }
    }
    void keyevent(int a,int b,int c,int d)
    {
        move=false;
        if( keyboard[a] )
        {
            velocity = velocity + PointD(0,-1)*speed;
            move = true;
        }
        if( keyboard[b] )
        {
            velocity = velocity + PointD(0,+1)*speed;
            move = true;
        }
        if( keyboard[c])
        {
            velocity = velocity + PointD(-1,0)*speed;
            move = true;
        }
        if( keyboard[d])
        {
            velocity = velocity + PointD(+1,0)*speed;
            move = true;
        }
        double len = velocity.length();
        if( len > speed )
        {
            velocity = velocity/len*speed;
        }
    }
    void new_pos()
    {
        //avoid player getting out of the boundary
        pos = pos + velocity;
        if(pos.x<0)
        {
            pos.x=0;
            velocity=PointD(0,0);
        }
        else if(pos.x>Game::SCREEN_WIDTH)
        {
            pos.x=Game::SCREEN_WIDTH;
            velocity=PointD(0,0);
        }
        else if(pos.y<0)
        {
            pos.y=0;
            velocity=PointD(0,0);
        }
        else if( pos.y>Game::SCREEN_HEIGHT)
        {
            pos.y=Game::SCREEN_HEIGHT;
            velocity=PointD(0,0);
        }
    }

};

class enemy:public sprite_t
{
public:
    enemy(const int &a )
    {
        pos=PointD(a,0) ;
        image=imageEnemy;
        Game::getImageSize(image,width,length);
        crashing_length=length;
        crashing_width=width;
        life=5;
        width*=0.7;
        velocity=PointD(0,2);
    }

    std::vector<fire> shootings;

};
class bullet:public sprite_t
{
public:
    template<class T>
    bullet(const T a,const int & i)
    {
        pos=a.pos;
        velocity.x=0;
        velocity.y=(i==0?8:4);
        image=(i==0?imagebullet_player:imagebullet_enemy);
        Game::getImageSize(image,width,length);
        crashing_length=length;
        crashing_width=width;
    }

};
class explosion:public sprite_t
{
public:
    template<class T>
    explosion(const T &a)
    {
        pos=a.pos;
        life=default_life;
        image=imageexplosion;
        Game::getImageSize(image,width,length);
        crashing_length=length;
        crashing_width=width;
    }

};
#endif // SPRITE_T_H_INCLUDED
