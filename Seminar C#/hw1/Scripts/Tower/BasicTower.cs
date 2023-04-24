
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UIElements;

public class BasicTower : Tower
{
    private bool AlreadyShooted = false;

    protected void Update()
    {
        if (!AlreadyShooted)
        {
            ShootOnNearestEnemy(base.DetectEnemiesInShootRange());
        }
        else
        {
            _timeSinceLastShot += Time.deltaTime;
            if (_timeSinceLastShot > TimeBetweenShoots)
            {
                ShootOnNearestEnemy(base.DetectEnemiesInShootRange());
            }
        }
    }


    protected void ShootOnNearestEnemy(List<GameObject> enemyGameObjects)
    {
        if (enemyGameObjects.Count == 0)
        {
            return;
        }
        GameObject nearestGameObject = FindNearestEnemy(enemyGameObjects);
        LazyEnemy lazyEnemy = nearestGameObject.GetComponent<LazyEnemy>();
        AggresiveEnemy aggresiveEnemy = nearestGameObject.GetComponent<AggresiveEnemy>();
        if (lazyEnemy != null || aggresiveEnemy != null)
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
            if (!AlreadyShooted)
            {
                AlreadyShooted = true;
            }
            _timeSinceLastShot = 0.0f;
        }
    }

    protected GameObject FindNearestEnemy(List<GameObject> enemyGameObjects)
    {
        float minDistance = float.MaxValue;
        GameObject nearestEnemy = null;
        foreach (GameObject enemyGameObject in enemyGameObjects)
        {
            float distance = Vector3.Distance(transform.position, enemyGameObject.transform.position);
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestEnemy = enemyGameObject;
            }
        }
        return nearestEnemy;
    }
}
