using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Mirror;

public class GunScript : MonoBehaviour
{
    [SerializeField] private GameObject bulletPrefab;
    [SerializeField] private Transform gunTip;
    public float fireRate = 1f;
    public AudioSource shootSource;
    public AudioClip shootSound;
    public TextMesh ammoText;

    public int maxAmmo = 10;
    public int currentAmmo;
    public float reloadTime = 1f;
    private bool isRelaoding = false;

    private float nextTimeToFire = 0f;

    void Start()
    {
        currentAmmo = maxAmmo;
    }

    void Update()
    {
        if (isRelaoding)
        {
            return;
        }
        
        
        if ((currentAmmo <= 0 || Input.GetKeyDown(KeyCode.R)) && !isRelaoding)
        {
            StartCoroutine(Reload());
        }
    }

    public void Shoot()
    {
        if (Time.time >= nextTimeToFire && currentAmmo > 0) {
            currentAmmo--;
            nextTimeToFire = Time.time + 1f / fireRate;
            GameObject bulletClone = Instantiate(bulletPrefab, gunTip.transform.position, gunTip.transform.rotation);
            NetworkServer.Spawn(bulletClone);
            shootSource.PlayOneShot(shootSound);
        }
    }
    
    IEnumerator Reload()
    {
        isRelaoding = true;
        yield return new WaitForSeconds(reloadTime);
        currentAmmo = maxAmmo;
        isRelaoding = false;
    }

    public void UpdateAmmoText()
    {
        ammoText.text = currentAmmo.ToString();
    }
}
