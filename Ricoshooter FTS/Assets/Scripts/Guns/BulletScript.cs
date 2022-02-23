using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BulletScript : MonoBehaviour
{
    private Rigidbody rb;
    Vector3 direction;
    float lastMagnitude;
    public int ricoshetes = 5;
    private float timeBeforeDeath = 10f;
    private float timeToDie;

    [SerializeField] float Speed = 10f;
    [SerializeField] float Damage = 25f;
    // Start is called before the first frame update
    void Awake()
    {
        rb = GetComponent<Rigidbody>();
        direction = transform.forward;
    }
    private void Start()
    {
        timeToDie = Time.time + timeBeforeDeath;
        rb.velocity = direction.normalized * Speed * Time.deltaTime;
        lastMagnitude = rb.velocity.magnitude;
        //Debug.Log("Start speed: " + direction * Speed * Time.deltaTime);
    }


    private void Update()
    {
        if(ricoshetes <= 0 || Time.time >= timeToDie)
        {
            Destroy(gameObject);
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            Destroy(gameObject);
            PlayerControllerRB_2 collController = collision.gameObject.GetComponent<PlayerControllerRB_2>();
            //collController.UpdateHealthbar(collController.hp - Damage);
            collController.TakeDamage(Damage);

            //collController.TakeDamage(Damage);
        }
        else
        {
            Reflect(collision);
        }
    }

    void Reflect(Collision collision)
    {
        if (collision.gameObject.layer == 6)
        {
            direction = Vector3.Reflect(direction.normalized, collision.contacts[0].normal);
            rb.velocity = direction * lastMagnitude;
            ricoshetes--;
        }
    }
}
