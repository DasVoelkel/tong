#include <GameAsset.hpp>

GameAsset::GameAsset(GameAssetAnimation *sprite_spawn, GameAssetAnimation *sprite_stat, GameAssetAnimation *sprite_death) : _sprite_spawn(sprite_spawn), _sprite_stationary(sprite_stat), _sprite_death(sprite_death)
{

    if (!_sprite_stationary)
    {
        fprintf(stderr, "No stationary sprite! ", _name);
        assert(false);
    }

    if (_sprite_spawn)
    {
        if (_sprite_spawn->get_sprite_size() < 2)
        {
            fprintf(stderr, "Spawn sprite has less than 2 frames! ", _name);
            assert(false);
        }
        _playing = PLAYING_TYPE::P_SPAWN;
    }

    if (_sprite_death)
    {
        if (_sprite_death->get_sprite_size() < 2)
        {
            fprintf(stderr, "death sprite has less than 2 frames! ", _name);
            assert(false);
        }
    }
}

void GameAsset::reset()
{
    bool _enable = true;

    PLAYING_TYPE _playing = PLAYING_TYPE::P_SPAWN;
    int _frame_counter = 0;
}

bool GameAsset::draw(ALLEGRO_DISPLAY *disp)
{
    if (!_enable)
    {
        return false;
    }

    switch (_playing)
    {
    case PLAYING_TYPE::P_SPAWN:

        if (_sprite_spawn->draw(disp, _coordinates, _frame_counter))
        {
            _frame_counter++;
        }
        else
        {
            _enable = false;
            return false;
        }

        if (_frame_counter > _sprite_spawn->get_sprite_size())
        {
            _playing = PLAYING_TYPE::P_STATIONARY;
            _frame_counter = 0;
        }

        break;
    case PLAYING_TYPE::P_STATIONARY:

        if (_sprite_stationary->draw(disp, _coordinates, _frame_counter))
        {
            _frame_counter++;
        }
        else
        {
            _enable = false;
            return false;
        }

        if (_frame_counter > _sprite_stationary->get_sprite_size())
        {
            _frame_counter = 0;
        }
        break;
    case PLAYING_TYPE::P_DEATH:

        if (_sprite_death->draw(disp, _coordinates, _frame_counter))
        {
            _frame_counter++;
        }
        else
        {
            _enable = false;
            return false;
        }

        if (_frame_counter > _sprite_death->get_sprite_size())
        {
            _frame_counter = 0;
            _enable = false;
        }
        break;
    }

    return true;
}

int GameAsset::get_height()
{
    return _sprite_stationary[1].get_height();
}
int GameAsset::get_width()
{
    return _sprite_stationary[1].get_width();
}

bool GameAsset::collision_with(GameAsset &obj)
{

    if (_coordinates.x > obj._coordinates.y + obj.get_height())
        return false;
    if (_coordinates.x + get_width() < obj._coordinates.x)
        return false;
    if (_coordinates.y > obj._coordinates.x + obj.get_width())
        return false;
    if (_coordinates.y + get_height() < obj._coordinates.y)
        return false;

    return true;
}