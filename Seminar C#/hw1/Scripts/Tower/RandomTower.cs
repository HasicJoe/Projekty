
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.ProBuilder;

public class RandomTower : Tower
{
    protected GameObject _selectedEnemy;
    protected bool _hasEnemy = false;
    

    protected void Update()
    {
        if (!_hasEnemy)
        {
            GameObject selectedGameObject = FindRandomEnemy(base.DetectEnemiesInShootRange());
            if (selectedGameObject != null)
            {
                _selectedEnemy = selectedGameObject;
                _hasEnemy = true;
                ShootOnEnemy();
            }
        }
        else
        {
            if (EnemyStillInRange())
            {
                ShootOnEnemy();
            }
            else
            {
                _selectedEnemy = null;
                _hasEnemy = false;
            }
        }
    }

    protected void ShootOnEnemy()
    {
        if (_selectedEnemy == null)
        {
            _hasEnemy = false;
            return;
        }
        LazyEnemy lazyEnemy = _selectedEnemy.GetComponent<LazyEnemy>();
        AggresiveEnemy aggresiveEnemy = _selectedEnemy.GetComponent<AggresiveEnemy>();
        if (lazyEnemy != null || aggresiveEnemy != null)
        {
            int strategy = ChooseShootStrategy();
            if (strategy == 1 && _timeSinceLastShot >= TimeBetweenShoots)
            {
                Projectile projectile = Instantiate(_projectilePrefab, transform.position, Quaternion.identity);
                if (lazyEnemy != null)
                {
                    projectile.StoreVictim(lazyEnemy.gameObject);
                    base.RotateTopToEnemy(lazyEnemy.gameObject);
                }
                else
                {
                    projectile.StoreVictim(aggresiveEnemy.gameObject);
                    base.RotateTopToEnemy(aggresiveEnemy.gameObject);
                }

                _timeSinceLastShot = 0.0f;
            }
            else if (strategy == 2 && _timeSinceLastShot >= TimeBetweenShoots)
            {
                Projectile projectile1 = Instantiate(_projectilePrefab, transform.position, Quaternion.identity);
                Projectile projectile2 = Instantiate(_projectilePrefab, transform.position, Quaternion.identity);
                if (lazyEnemy != null)
                {
                    projectile1.StoreVictim(lazyEnemy.gameObject);
                    projectile2.StoreVictim(lazyEnemy.gameObject);
                    base.RotateTopToEnemy(lazyEnemy.gameObject);
                }
                else
                {
                    projectile1.StoreVictim(aggresiveEnemy.gameObject);
                    projectile2.StoreVictim(aggresiveEnemy.gameObject);
                    base.RotateTopToEnemy(aggresiveEnemy.gameObject);
                }
                _timeSinceLastShot = 0.0f;
            }
            else
            {
                _timeSinceLastShot += Time.deltaTime;
            }
        }
    }


    protected bool EnemyStillInRange()
    {
        Collider[] colliders = Physics.OverlapSphere(transform.position, ShootRange);
        foreach (Collider collider in colliders)
        {
            if (collider.gameObject == _selectedEnemy)
            {
                return true;
            }
        }
        return false;
    }


    protected int ChooseShootStrategy()
    {
        // 1 = 60% shoot one projectile, 2 = 20% shoot two projectiles, 3 = 20% do not shoot
        List<int> strategies = new List<int>() {1, 1, 1, 2, 3};
        System.Random random = new System.Random();
        int index = random.Next(strategies.Count);
        return strategies[index];
    }


    protected GameObject FindRandomEnemy(List<GameObject> enemies)
    {
        if (enemies.Count == 0)
        {
            return null;
        }
        System.Random random = new System.Random();
        int index = random.Next(enemies.Count);
        return enemies[index];
    }

}
