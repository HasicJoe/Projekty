using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.SearchService;
using UnityEngine;

[RequireComponent(typeof(BoxCollider), typeof(HealthComponent))]
public class Tower : MonoBehaviour
{
    [SerializeField] protected LayerMask _enemyLayerMask;
    [SerializeField] private HealthComponent _healthComponent;
    [SerializeField] protected Projectile _projectilePrefab;
    [SerializeField] private BoxCollider _boxCollider;
    [SerializeField] protected Transform _objectToPan;
    [SerializeField] protected Transform _projectileSpawn;
    [SerializeField] private GameObject _previewPrefab;

    public HealthComponent Health => _healthComponent;
    public BoxCollider Collider => _boxCollider;
    public GameObject BuildingPreview => _previewPrefab;

    // TODO: Modify so they have appropriate price
    public int Price;
    public int ShootRange;
    public string Name;
    public float TimeBetweenShoots;
    public int NumberOfLifes;
    protected float _timeSinceLastShot = 0.0f;

    private void Start()
    {
        _healthComponent.OnDeath += HandleDeath;
    }


    private void OnDestroy()
    {
        _healthComponent.OnDeath -= HandleDeath;
    }

    private void HandleDeath()
    {
        Destroy(gameObject);
    }

    protected void RotateTopToEnemy(GameObject enemyGameObject)
    {
      
        _objectToPan.rotation = Quaternion.LookRotation(Vector3.Normalize(enemyGameObject.transform.position - transform.position));
        Vector3 direction = enemyGameObject.transform.position - transform.position;
        transform.rotation = Quaternion.RotateTowards(_objectToPan.rotation, Quaternion.LookRotation(direction),
            5.0f * Time.deltaTime);
    }


    protected List<GameObject> DetectEnemiesInShootRange()
    {
        List<GameObject> enemyGameObjects = new List<GameObject>() { };
        Collider[] colliders = Physics.OverlapSphere(transform.position, ShootRange);
        foreach (Collider collider in colliders)
        {
            if (collider.gameObject.GetComponent<LazyEnemy>() != null ||
                collider.gameObject.GetComponent<AggresiveEnemy>() != null)
            {
                enemyGameObjects.Add(collider.gameObject);
            }
        }
        return enemyGameObjects;
    }
}
