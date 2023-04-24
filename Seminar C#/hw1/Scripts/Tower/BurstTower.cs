
using System.Collections.Generic;
using TMPro.EditorUtilities;
using UnityEngine;

public class BurstTower : Tower
{
    protected GameObject _selectedEnemy;
    protected bool _hasEnemy = false;
    protected int _initialShoots = 0;

    protected void Update()
    {
        if (!_hasEnemy)
        {
            GameObject enemyGameObject = SelectEnemyWithMostHealth(base.DetectEnemiesInShootRange());
            if (enemyGameObject != null)
            {
                _selectedEnemy = enemyGameObject;
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
                _initialShoots = 0;
            }
        }
    }


    protected void ShootOnEnemy()
    {
        if (_selectedEnemy == null)
        {
            return;
        }
        LazyEnemy lazyEnemy = _selectedEnemy.GetComponent<LazyEnemy>();
        AggresiveEnemy aggresiveEnemy = _selectedEnemy.GetComponent<AggresiveEnemy>();
        if (lazyEnemy != null || aggresiveEnemy != null)
        {
            if ((_initialShoots < 2 && _timeSinceLastShot >= 0.2f) || (_initialShoots == 2 && _timeSinceLastShot >= TimeBetweenShoots))
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
                if (_initialShoots < 2)
                {
                    _initialShoots += 1;
                }
            }
            else if ((_initialShoots < 2 && _timeSinceLastShot < 0.2f) || (_initialShoots == 2 && _timeSinceLastShot < TimeBetweenShoots))
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


    protected GameObject SelectEnemyWithMostHealth(List<GameObject> enemies)
    {
        float maxHealth = float.MinValue;
        GameObject enemyMaxHealth = null;
        foreach (GameObject enemy in enemies)
        {
            if (enemy.GetComponent<LazyEnemy>() != null)
            {
                LazyEnemy lazyEnemy = enemy.GetComponent<LazyEnemy>();
                if (lazyEnemy.GetHealth() > maxHealth)
                {
                    maxHealth = lazyEnemy.GetHealth();
                    enemyMaxHealth = enemy;
                }
            }
            else
            {
                AggresiveEnemy aggresiveEnemy = enemy.GetComponent<AggresiveEnemy>();
                if (aggresiveEnemy.GetHealth() > maxHealth)
                {
                    maxHealth = aggresiveEnemy.GetHealth();
                    enemyMaxHealth = enemy;
                }
            }
        }
        return enemyMaxHealth;
    }
}
