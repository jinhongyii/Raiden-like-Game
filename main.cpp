#include "SDL2_header.h"
#include"sprite_t.h"
const std::string Game::TitleName = "lightning";
using namespace Game;

std::default_random_engine e;
std::uniform_int_distribution<unsigned> u(30,SCREEN_WIDTH-30);
std::vector<player> players;
std::vector<enemy> enemies;
std::vector<bullet> bullets_player[2];
std::vector<explosion> explosions;
std::vector<bullet> bullets_enemy;


void loadwav()
{
    soundexplosion=loadmusic("boom.wav");
    soundbullet=loadmusic("bullet.wav");
    sounddie=loadmusic("me_down.wav");
    soundhurt=loadmusic("car_door.wav");
    soundenemyhurt=loadmusic("enemy2_down.wav");
    soundsuperweapon=loadmusic("get_bomb.wav");
}

void loadPictures()
{
    imagePlayer[0] = loadImage( "player.png"	);
    imagebullet_player = loadImage( "bullet.png"	);
    imageEnemy	= loadImage( "player_u.png" );
    imageexplosion=loadImage("explosion1.gif");
    imagebullet_enemy =loadImage("bullet2.png");
    imagePlayer[1]=loadImage("d1.png");
}

void initialize()
{
    //Display FPS
    FPS_DISPLAY = true;
    //Load pictures from files
    loadPictures();
    //Initialize vairables
    for(int i=0; i<2; i++)
    {
        players.push_back(player(0));
        players.push_back(player(1));
    }

    canvasColor = {0, 0, 0, 255};
    e.seed(time(NULL));
    //open sdlmixer
    int result=Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 );
    // Check load
    if ( result != 0 )
    {
        std::cout << "Failed to open audio: " << Mix_GetError() << std::endl;
    }
    // play game music

    char address[100],b[100];
    std::stringstream ss;
    ss<<RES_PATH_MUS;
    ss>>b;
    ss.clear();
    strcpy(address,b);
    strcat(address,"game_music.ogg");
    sound=Mix_LoadMUS(address);
    result=Mix_PlayMusic(sound,-1);
    if ( result != 0 )
    {
        std::cout << "Failed to play music: " << Mix_GetError() << std::endl;
    }
    //load sound effect
    Mix_AllocateChannels(16);
    loadwav();
}
//draw all players
void drawPlayer()
{
    for(int i=0; i<2; i++)
    {
        players[i].draw();
    }

}


void drawHint()
{
    drawtext("player1:  life:"+std::to_string(players[0].life)+" "+"score:"+std::to_string(players[0].score)+" superweapons:"+std::to_string(players[0].superweapons), SCREEN_WIDTH/2, SCREEN_HEIGHT-30);
    drawtext("player2:  life:"+std::to_string(players[1].life)+" "+"score:"+std::to_string(players[1].score)+" superweapons:"+std::to_string(players[1].superweapons), SCREEN_WIDTH/2, SCREEN_HEIGHT );
}
void  drawgameover(int time,int i)
{
    //i is the number of the player(0 or 1)
    if(i==0)
    {
        drawtext("Game Over", SCREEN_WIDTH/2, SCREEN_HEIGHT/3,2,2);

    }
    drawtext("player"+std::to_string(i+1)+" kill "+std::to_string(players[i].score/10)+" enemies",SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    drawtext("player"+std::to_string(i+1)+" die "+std::to_string(5-players[i].life)+" times"+" ", SCREEN_WIDTH/2, SCREEN_HEIGHT/2+30 );
    //temp is the score of player i
    int temp=int(double(players[i].score)/10/(time/120)*100-1*(5-players[i].life))*2;
    if(temp<0)
    {
        temp=0;
    }
    drawtext("performance:"+std::to_string(temp), SCREEN_WIDTH/2, SCREEN_HEIGHT/2+60);
}
//draw all the bullets
void drawBullet()
{
    for(int j=0; j<2; j++)
    {
        for(int i=0; i<bullets_player[j].size(); i++)
        {
            bullets_player[j][i].draw();
        }
    }
    for(int i=0; i<bullets_enemy.size(); i++)
    {
        bullets_enemy[i].draw();
    }
}
//draw all the enemies
void drawEnemy()
{
    for(int i=0; i<enemies.size(); i++)
    {
        enemies[i].draw();
    }
}
//draw all the explosions
void drawExplosion()
{
    for(int i=0; i<explosions.size(); i++)
    {
        explosions[i].draw();
    }
}
//the following lines remains to be developed
//bool pause=false;
/*void draw_pause(){

    drawtext("continue?",SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

}*/
void draw()
{
    drawPlayer();
    drawBullet();
    drawEnemy();
    drawHint();
    drawExplosion();
    /*if(pause){
        draw_pause();
        return;
    } */
}


void deal(int &count)
{
    /*if(keyboard[KEY_ESC] and count%10==0){
        pause=!pause;
    }
    if(pause){
        count--;

    } */
    //poll key events
    //move
    players[0].keyevent(KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT);
    players[1].keyevent('w','s','a','d');
    //bullets
    if(keyboard[KEY_SPACE] and count%6==0 and players[0].alive())
    {
        bullets_player[0].push_back(bullet(players[0].pos.x,players[0].pos.y,0)) ;
        playmusic(soundbullet);
    }
    if(keyboard['q'] and count%6==1 and players[1].alive())
    {
        bullets_player[1].push_back(bullet(players[1].pos.x,players[1].pos.y,0));
        playmusic(soundbullet);
    }
    //launch super weapon
    if(keyboard['c'] and players[0].superweapons and players[0].cooling_down<=0 and players[0].alive())
    {
        players[0].score+=10*enemies.size();
        for(int i=0; i<enemies.size(); i++)
        {
            explosions.push_back(explosion(enemies[i].pos));
        }
        bullets_enemy.clear();
        enemies.clear();
        players[0].superweapons--;
        players[0].cooling_down=600;
        playmusic(soundsuperweapon);
    }
    if(keyboard['x'] and players[1].superweapons and players[1].cooling_down<=0 and players[1].alive())
    {
        players[1].score+=10*enemies.size();
        for(int i=0; i<enemies.size(); i++)
        {
            explosions.push_back(explosion(enemies[i].pos));
        }
        bullets_enemy.clear();
        enemies.clear();
        players[1].superweapons--;
        players[1].cooling_down=600;
        playmusic(soundsuperweapon);
    }
    //add new enemy and new bullet
    if(count%120==1)
    {
        enemies.push_back(enemy(u(e)));
    }
    if(count%60==1)
    {
        for(int i=0; i<enemies.size(); i++)
        {
            bullets_enemy.push_back(bullet(enemies[i].pos.x,enemies[i].pos.y,1));
        }
    }
    //Calculate new position of all sprites
    //restrict player's move
    for(int i=0; i<2; i++)
    {
        players[i].new_pos();
        for(int j=0; j<bullets_player[i].size(); j++)
        {
            bullets_player[i][j].pos =bullets_player[i][j].pos-bullets_player[i][j].velocity;
        }
    }
    for(int i=0; i<enemies.size(); i++)
    {
        enemies[i].pos=enemies[i].pos+enemies[i].velocity;
    }
    for(int i=0; i<bullets_enemy.size(); i++)
    {
        bullets_enemy[i].pos=bullets_enemy[i].pos+bullets_enemy[i].velocity;
    }
    //detect collide
    for(int k=0; k<2; k++)
    {
        for(int i=0; i<enemies.size(); i++)
        {
            for(int j=0; j<bullets_player[k].size(); j++)
            {
                if(collide(enemies[i],bullets_player[k][j]))
                {
                    explosions.push_back(explosion(enemies[i].pos));
                    enemies[i].life--;

                    bullets_player[k].erase(bullets_player[k].begin()+j);
                    playmusic(soundenemyhurt);
                    if(!enemies[i].alive())
                    {
                        players[k].score+=10;

                        enemies.erase(enemies.begin()+i);
                        playmusic(soundexplosion);
                        i--;
                        break;
                    }
                    j--;
                }
            }
        }


        for(int i=0; i<enemies.size(); i++)
        {
            if(collide(enemies[i],players[k]) and !players[k].reborn and players[k].alive())
            {
                explosions.push_back(explosion(enemies[i].pos));

                enemies.erase(enemies.begin()+i);
                players[k].life--;
                if(!players[k].alive())
                {
                    playmusic(sounddie);
                    break;
                }
                players[k].reborn=true;
                players[k].supertime=120;
                playmusic(soundhurt);
                break;
            }
        }

        for(int i=0; i<bullets_enemy.size(); i++)
        {
            if(collide2(bullets_enemy[i],players[k]) and !players[k].reborn and players[k].alive())
            {
                explosions.push_back(explosion(players[k].pos));

                bullets_enemy.erase(bullets_enemy.begin()+i);
                players[k].life--;
                if(!players[k].alive())
                {
                    playmusic(sounddie);
                    break;
                }
                players[k].reborn=true;
                players[k].supertime=120;
                playmusic(soundhurt);
                break;
            }
        }
    }
    //update status and remove dead sprite
    for(int i=0; i<explosions.size(); i++)
    {
        explosions[i].life--;
        if(!explosions[i].alive())
        {

            explosions.erase(explosions.begin()+i);
            i--;
        }
    }
    for(int i=0; i<enemies.size(); i++)
    {
        if(enemies[i].pos.y>=SCREEN_HEIGHT)
        {

            enemies.erase(enemies.begin()+i);
            i--;
        }
    }
    for(int k=0; k<2; k++)
    {
        for(int i=0; i<bullets_player[k].size(); i++)
        {
            if(bullets_player[k][i].pos.y<=0)
            {

                bullets_player[k].erase(bullets_player[k].begin()+i);
                i--;
            }
        }
    }
    for(int i=0; i<bullets_enemy.size(); i++)
    {
        if(bullets_enemy[i].pos.y>=SCREEN_HEIGHT)
        {

            bullets_enemy.erase(bullets_enemy.begin()+i);
            i--;
        }
    }
    for(int i=0; i<2; i++)
    {
        players[i].update();
    }

}
//record the stop time of the game
int stop_time;
//flag records whether the stop time has been recorded
bool flag=true;
int work( bool &quit )
{
    //Calculate time
    static int count=0;
    count++;
    if(((!players[0].alive() and !players[1].alive())  or count>3600) and flag)
    {
        stop_time=count;
        flag=false;
    }
    if(count<=3600 and (players[0].alive() or players[1].alive()))
    {
        deal(count);
        //Draw on the screen
        draw();
    }
    else if (count<=stop_time+300)
    {
        //draw game over status of player 0
        drawgameover(stop_time,0);
    }
    else
    {
        //draw game over status of player 1
        drawgameover(stop_time,1);
    }
    //if( keyboard[KEY_ESC]  )
    //quit = true;
    return 0;
}

void mousePress()
{
}

void mouseMove()
{

}

void mouseRelease()
{

}

void finale()
{
    //Delete all images and delete all sounds;
    cleanup(imagePlayer[0],imagePlayer[1], imagebullet_player, imageEnemy,imageexplosion,imagebullet_enemy );
    Mix_FreeChunk(soundbullet);
    Mix_FreeChunk(sounddie);
    Mix_FreeChunk(soundenemyhurt);
    Mix_FreeChunk(soundexplosion);
    Mix_FreeChunk(soundhurt);
    Mix_FreeChunk(soundsuperweapon);
    Mix_FreeMusic(sound);
}


